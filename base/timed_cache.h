#pragma once

#include "base/any_executor.h"
#include "base/any_executor_timer.h"
#include "base/check.h"
#include "base/time/time.h"
#include "base/time_utils.h"

#include <chrono>
#include <concepts>
#include <map>
#include <memory>

template <class Value>
bool IsTimedCacheExpired(const Value& value);

template <class Key, class Value>
  requires std::totally_ordered<Key>
class TimedCache {
 public:
  explicit TimedCache(AnyExecutor executor);

  template <class T>
  void Add(const Key& key, T&& value);

  Value Find(const Key& key) const;

 private:
  struct CacheEntry {
    template <class T>
    explicit CacheEntry(T&& value) : value{std::forward<T>(value)} {}

    Value value;
    std::chrono::steady_clock::time_point expiration_time;
  };

  void OnTimer();

  const scada::base::TimeDelta cache_duration_ =
      std::chrono::seconds{kCacheDurationS};

  std::map<Key, CacheEntry> map_;

  AnyExecutorTimer timer_;

#ifdef _DEBUG
  static constexpr unsigned kCacheDurationS = 10;
#else
  static constexpr unsigned kCacheDurationS = 60;
#endif
};

template <class Key, class Value>
  requires std::totally_ordered<Key>
inline TimedCache<Key, Value>::TimedCache(AnyExecutor executor)
    : timer_{std::move(executor)} {
  using namespace std::chrono_literals;
  timer_.StartRepeating(1s, [this] { OnTimer(); });
}

template <class Key, class Value>
  requires std::totally_ordered<Key>
inline Value TimedCache<Key, Value>::Find(const Key& key) const {
  auto i = map_.find(key);
  return i != map_.end() ? i->second.value : nullptr;
}

template <class Key, class Value>
  requires std::totally_ordered<Key>
template <class T>
inline void TimedCache<Key, Value>::Add(const Key& key, T&& value) {
  scada::base::Check(map_.find(key) == map_.end());
  map_.emplace(std::piecewise_construct, std::forward_as_tuple(key),
               std::forward_as_tuple(std::forward<T>(value)));
}

template <class Key, class Value>
  requires std::totally_ordered<Key>
inline void TimedCache<Key, Value>::OnTimer() {
  std::chrono::steady_clock::time_point time =
      std::chrono::steady_clock::now();
  for (auto i = map_.begin(); i != map_.end();) {
    auto& entry = i->second;
    if (IsTimedCacheExpired(entry.value)) {
      if (entry.expiration_time == std::chrono::steady_clock::time_point{}) {
        // Expiration timer started.
        entry.expiration_time = std::chrono::steady_clock::now();
      } else if (time - entry.expiration_time >= cache_duration_) {
        // Expired.
        map_.erase(i++);
        continue;
      }
    } else {
      entry.expiration_time = {};
    }
    ++i;
  }
}
