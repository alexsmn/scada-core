#pragma once

#include "base/any_executor.h"
#include "base/awaitable.h"
#include "base/callback_awaitable.h"

#include <cassert>
#include <functional>
#include <map>
#include <optional>
#include <vector>

namespace base {

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

  template <class OnWaiter>
  [[nodiscard]] Awaitable<Value> Wait(Key key, OnWaiter&& on_waiter) {
    auto [value] = co_await CallbackToAwaitable<Value>(
        executor_, [this, key = std::move(key),
                    on_waiter = std::forward<OnWaiter>(on_waiter)](
                       auto callback) mutable {
          auto completion =
              std::make_shared<std::decay_t<decltype(callback)>>(
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

  void Complete(const Key& key, Value value) {
    auto i = entries_.find(key);
    assert(i != entries_.end());
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

}  // namespace base
