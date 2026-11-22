#include "remote/login_throttle.h"

#include <algorithm>
#include <utility>

namespace {

constexpr std::string_view kUserPrefix = "user:";
constexpr std::string_view kAddressPrefix = "addr:";

std::string MakeKey(std::string_view prefix, std::string_view value) {
  std::string key;
  key.reserve(prefix.size() + value.size());
  key.append(prefix);
  key.append(value);
  return key;
}

}  // namespace

LoginThrottle::LoginThrottle(LoginThrottlePolicy policy,
                             NowFunction now,
                             SleepFunction sleep)
    : policy_{std::move(policy)},
      now_{std::move(now)},
      sleep_{std::move(sleep)} {}

LoginThrottle::Admission LoginThrottle::BeginAttempt(const Keys& keys) {
  const TimePoint now = now_();

  KeyState* user = keys.user_name.empty()
                       ? nullptr
                       : &Touch(MakeKey(kUserPrefix, keys.user_name), now);
  KeyState* address = keys.address.empty()
                          ? nullptr
                          : &Touch(MakeKey(kAddressPrefix, keys.address), now);

  const std::size_t prior_failures =
      std::max(user ? user->failures : 0, address ? address->failures : 0);
  const Clock::duration delay = DelayFor(prior_failures);

  if (address && address->locked_until > now) {
    return {.delay = delay, .locked_out = true};
  }

  for (KeyState* state : {user, address}) {
    if (state) {
      ++state->failures;
      state->last_attempt = now;
    }
  }

  if (address && policy_.lockout_failures != 0 &&
      address->failures >= policy_.lockout_failures) {
    // This attempt still goes ahead; the ones after it are refused until the
    // window passes. After that the count stays at the threshold, so each
    // further failure re-locks at once — one guess per window — until a
    // success or `forget_after` clears it.
    address->locked_until = now + policy_.lockout_window;
  }

  return {.delay = delay, .locked_out = false};
}

void LoginThrottle::RecordSuccess(const Keys& keys) {
  if (!keys.user_name.empty()) {
    keys_.erase(MakeKey(kUserPrefix, keys.user_name));
  }
  if (!keys.address.empty()) {
    keys_.erase(MakeKey(kAddressPrefix, keys.address));
  }
}

Awaitable<void> LoginThrottle::Sleep(Clock::duration delay) const {
  if (sleep_ && delay > Clock::duration::zero()) {
    co_await sleep_(delay);
  }
}

LoginThrottle::KeyState& LoginThrottle::Touch(const std::string& key,
                                              TimePoint now) {
  if (auto i = keys_.find(key); i != keys_.end()) {
    KeyState& state = i->second;
    if (state.locked_until <= now &&
        now - state.last_attempt >= policy_.forget_after) {
      state = KeyState{};
    }
    return state;
  }

  MakeRoom(now);
  return keys_[key];
}

Clock::duration LoginThrottle::DelayFor(std::size_t failures) const {
  if (failures < policy_.free_failures) {
    return Clock::duration::zero();
  }
  Clock::duration delay = policy_.base_delay;
  for (std::size_t i = policy_.free_failures; i < failures; ++i) {
    if (delay >= policy_.max_delay) {
      break;
    }
    delay *= 2;
  }
  return std::min(delay, policy_.max_delay);
}

void LoginThrottle::MakeRoom(TimePoint now) {
  if (keys_.size() < policy_.max_tracked_keys) {
    return;
  }

  // Idle keys first: they would be reset on their next attempt anyway.
  std::erase_if(keys_, [&](const auto& item) {
    const KeyState& state = item.second;
    return state.locked_until <= now &&
           now - state.last_attempt >= policy_.forget_after;
  });

  // Then the least recently used, preferring keys that are not locked out.
  while (!keys_.empty() && keys_.size() >= policy_.max_tracked_keys) {
    auto victim = std::min_element(
        keys_.begin(), keys_.end(), [&](const auto& a, const auto& b) {
          const bool a_locked = a.second.locked_until > now;
          const bool b_locked = b.second.locked_until > now;
          if (a_locked != b_locked) {
            return !a_locked;
          }
          return a.second.last_attempt < b.second.last_attempt;
        });
    keys_.erase(victim);
  }
}

std::string LoginThrottlePeerAddress(std::string_view peer) {
  if (peer.starts_with('[')) {
    const auto close = peer.find(']');
    if (close != std::string_view::npos) {
      return std::string{peer.substr(1, close - 1)};
    }
    return std::string{peer};
  }

  const auto colon = peer.rfind(':');
  // More than one colon without brackets is a bare IPv6 address: no port.
  if (colon == std::string_view::npos || peer.find(':') != colon) {
    return std::string{peer};
  }
  return std::string{peer.substr(0, colon)};
}
