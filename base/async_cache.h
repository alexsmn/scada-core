#pragma once

#include "base/any_executor.h"
#include "base/awaitable.h"
#include "base/callback_awaitable.h"
#include "base/check.h"

#include <functional>
#include <map>
#include <optional>
#include <vector>

namespace scada::base {

// Executor-affine async result cache.
//
// `AsyncCache` deduplicates concurrent waiters for the same key and stores the
// completed value for later waiters. The owner decides when a miss should start
// by calling `TryStart(key)` from the `Wait(...)` miss callback or from a later
// batching point, then calls `Complete(key, value)` when the async fetch
// finishes. `AsyncCache` intentionally has no eviction policy; use it for
// request-scoped or otherwise bounded fan-in, and keep a separate MRU cache
// where bounded recency semantics matter.
//
// Example:
//   base::AsyncCache<NodeId, StatusOr<NodeStatePtr>> cache{executor};
//   auto result = co_await cache.Wait(node_id, [&](const NodeId& key) {
//     if (cache.TryStart(key)) {
//       CoSpawn(executor, [&cache, key]() -> Awaitable<void> {
//         cache.Complete(key, co_await FetchNodeState(key));
//       });
//     }
//   });
template <class Key, class Value, class Compare = std::less<Key>>
class AsyncCache {
 public:
  using Handler = std::function<void(Value)>;

  explicit AsyncCache(AnyExecutor executor) : executor_{std::move(executor)} {}

  // `on_waiter` is taken BY VALUE, and must be: this is a lazy coroutine, whose
  // frame copies a reference parameter as a reference. It took `OnWaiter&&`
  // until 2026-09-27, so `StartAwaitable(executor, cache.Wait(k, [&]{...}))`
  // -- anything but `co_await` in the same full expression -- destroyed the
  // lambda at the end of the statement and then ran the body against it,
  // which was an access violation on Windows
  // (AsyncCache.WaitersShareOneStartedFetch, scada-core run 36324571715)
  // and passed by luck elsewhere.
  template <class OnWaiter>
  [[nodiscard]] Awaitable<Value> Wait(Key key, OnWaiter on_waiter) {
    auto [value] = co_await CallbackToAwaitable<Value>(
        executor_,
        [this, key = std::move(key),
         on_waiter = std::move(on_waiter)](auto callback) mutable {
          auto completion = std::make_shared<std::decay_t<decltype(callback)>>(
              std::move(callback));
          auto& entry = entries_[key];

          if (entry.value.has_value()) {
            (*completion)(*entry.value);
            return;
          }

          entry.waiters.emplace_back([completion](Value value) mutable {
            (*completion)(std::move(value));
          });
          std::invoke(on_waiter, key);
        });
    co_return std::move(value);
  }

  [[nodiscard]] std::vector<Key> PendingKeys() const {
    std::vector<Key> result;
    for (const auto& [key, entry] : entries_) {
      if (!entry.value.has_value() && !entry.in_progress &&
          !entry.waiters.empty()) {
        result.emplace_back(key);
      }
    }
    return result;
  }

  [[nodiscard]] bool TryStart(const Key& key) {
    auto& entry = entries_[key];
    if (entry.value.has_value() || entry.in_progress || entry.waiters.empty()) {
      return false;
    }

    entry.in_progress = true;
    return true;
  }

  // Claims `key` for a fetch that no one is waiting on yet, so an owner that
  // already knows which keys a request will need can read them in one batch
  // ahead of the code that consumes them one at a time. Returns false when the
  // key already has a value or a fetch in flight.
  //
  // This is `TryStart` without its "somebody is waiting" precondition: the
  // caller must `Complete` every key it claims, including on failure, or a
  // waiter arriving later never wakes — `TryStart` would decline the key
  // because the prefetch left it in progress.
  [[nodiscard]] bool TryStartPrefetch(const Key& key) {
    auto& entry = entries_[key];
    if (entry.value.has_value() || entry.in_progress) {
      return false;
    }

    entry.in_progress = true;
    return true;
  }

  void Complete(const Key& key, Value value) {
    auto i = entries_.find(key);
    base::Check(i != entries_.end());
    if (i == entries_.end()) {
      return;
    }

    auto& entry = i->second;
    entry.in_progress = false;
    entry.value = std::move(value);

    auto waiters = std::move(entry.waiters);
    for (auto& waiter : waiters) {
      waiter(*entry.value);
    }
  }

 private:
  struct Entry {
    std::optional<Value> value;
    bool in_progress = false;
    std::vector<Handler> waiters;
  };

  AnyExecutor executor_;
  std::map<Key, Entry, Compare> entries_;
};

}  // namespace scada::base
