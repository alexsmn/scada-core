#pragma once

#include <cassert>
#include <list>
#include <map>
#include <unordered_map>

namespace base {

// A most-recently-used cache, compatible with the ChromiumBase API.
// KeyType must be comparable with the MapType's key_compare.
template <class KeyType, class ValueType, class MapType>
class MRUCacheBase {
 public:
  using value_type = std::pair<KeyType, ValueType>;

 private:
  using PayloadList = std::list<value_type>;

 public:
  using iterator = typename PayloadList::iterator;
  using const_iterator = typename PayloadList::const_iterator;
  using reverse_iterator = typename PayloadList::reverse_iterator;

  explicit MRUCacheBase(size_t max_size) : max_size_{max_size} {
    assert(max_size > 0);
  }

  MRUCacheBase(const MRUCacheBase&) = delete;
  MRUCacheBase& operator=(const MRUCacheBase&) = delete;

  size_t size() const { return index_.size(); }
  size_t max_size() const { return max_size_; }

  iterator Put(const KeyType& key, const ValueType& value) {
    auto i = index_.find(key);
    if (i != index_.end()) {
      // Update existing entry and move to front.
      ordering_.erase(i->second);
      index_.erase(i);
    }
    ordering_.emplace_front(key, value);
    index_.emplace(key, ordering_.begin());

    // Evict oldest if over capacity.
    while (ordering_.size() > max_size_) {
      auto last = ordering_.end();
      --last;
      index_.erase(last->first);
      ordering_.erase(last);
    }

    return ordering_.begin();
  }

  iterator Put(const KeyType& key, ValueType&& value) {
    auto i = index_.find(key);
    if (i != index_.end()) {
      ordering_.erase(i->second);
      index_.erase(i);
    }
    ordering_.emplace_front(key, std::move(value));
    index_.emplace(key, ordering_.begin());

    while (ordering_.size() > max_size_) {
      auto last = ordering_.end();
      --last;
      index_.erase(last->first);
      ordering_.erase(last);
    }

    return ordering_.begin();
  }

  // Get moves the element to the front (most recently used).
  iterator Get(const KeyType& key) {
    auto i = index_.find(key);
    if (i == index_.end())
      return end();
    auto payload_iter = i->second;
    ordering_.splice(ordering_.begin(), ordering_, payload_iter);
    return payload_iter;
  }

  // Peek does not move the element.
  iterator Peek(const KeyType& key) {
    auto i = index_.find(key);
    if (i == index_.end())
      return end();
    return i->second;
  }

  const_iterator Peek(const KeyType& key) const {
    auto i = index_.find(key);
    if (i == index_.end())
      return end();
    return i->second;
  }

  iterator Erase(iterator pos) {
    index_.erase(pos->first);
    return ordering_.erase(pos);
  }

  iterator begin() { return ordering_.begin(); }
  iterator end() { return ordering_.end(); }
  const_iterator begin() const { return ordering_.begin(); }
  const_iterator end() const { return ordering_.end(); }
  reverse_iterator rbegin() { return ordering_.rbegin(); }
  reverse_iterator rend() { return ordering_.rend(); }

 private:
  PayloadList ordering_;
  MapType index_;
  size_t max_size_;
};

// MRUCache uses std::map (ordered by key).
template <class KeyType, class ValueType>
class MRUCache
    : public MRUCacheBase<
          KeyType,
          ValueType,
          std::map<KeyType,
                   typename std::list<std::pair<KeyType, ValueType>>::iterator>> {
  using Base = MRUCacheBase<
      KeyType,
      ValueType,
      std::map<KeyType,
               typename std::list<std::pair<KeyType, ValueType>>::iterator>>;

 public:
  using Base::Base;
};

// HashingMRUCache uses std::unordered_map (hashed by key).
template <class KeyType, class ValueType>
class HashingMRUCache
    : public MRUCacheBase<
          KeyType,
          ValueType,
          std::unordered_map<
              KeyType,
              typename std::list<std::pair<KeyType, ValueType>>::iterator>> {
  using Base = MRUCacheBase<
      KeyType,
      ValueType,
      std::unordered_map<
          KeyType,
          typename std::list<std::pair<KeyType, ValueType>>::iterator>>;

 public:
  using Base::Base;
};

}  // namespace base
