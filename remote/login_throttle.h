#pragma once

#include "base/awaitable.h"
#include "base/common_types.h"

#include <chrono>
#include <cstddef>
#include <functional>
#include <string>
#include <string_view>
#include <unordered_map>

// How LoginThrottle slows down and locks out repeated failed logins on the
// remote («Телеконтроль») protocol.
//
// The values are constants rather than settings because the remote module's
// configuration is only its endpoint list (a string or an array of strings in
// server.json's "sessions"), so there is no existing place to put them.
struct LoginThrottlePolicy {
  // Failed attempts a key may make before attempts start being delayed, so an
  // operator who mistypes a password once or twice notices nothing.
  std::size_t free_failures = 3;
  // Delay before the first delayed attempt; each further failure doubles it,
  // up to `max_delay`.
  Clock::duration base_delay = std::chrono::seconds{1};
  Clock::duration max_delay = std::chrono::seconds{30};
  // Failed attempts from one remote ADDRESS that lock that address out for
  // `lockout_window`. Zero disables the lockout. Only addresses are locked out,
  // never accounts: an account lockout would let anyone deny any account —
  // root included — by guessing at it, so a user name is only ever delayed.
  std::size_t lockout_failures = 10;
  Clock::duration lockout_window = std::chrono::minutes{5};
  // A key with no attempt for this long starts again from zero.
  Clock::duration forget_after = std::chrono::hours{1};
  // Upper bound on tracked keys, so a stream of distinct user names or
  // addresses cannot grow the table without limit.
  std::size_t max_tracked_keys = 4096;
};

// Counts failed logins per user name and per remote address, and tells the
// login path how long to wait before authenticating and whether to refuse the
// attempt outright.
//
// An attempt is counted as a failure when it BEGINS and forgiven when it
// succeeds. Counting up front is what makes concurrent attempts pay: a client
// that opens many connections at once sees each attempt find the counts its
// predecessors already raised, instead of all of them reading zero.
//
// Single-threaded: call it from the session manager's executor only.
class LoginThrottle {
 public:
  using NowFunction = std::function<TimePoint()>;
  using SleepFunction = std::function<Awaitable<void>(Clock::duration)>;

  // Who is logging in. Either part may be empty, in which case it is not
  // tracked (an address is empty when the transport cannot name its peer).
  struct Keys {
    std::string user_name;
    std::string address;
  };

  // What BeginAttempt decided.
  struct Admission {
    // How long to wait before authenticating (or before refusing).
    Clock::duration delay{};
    // The address is locked out: refuse without authenticating.
    bool locked_out = false;
  };

  // `sleep` waits out a delay; it is a parameter so a test can complete it
  // without waiting real time.
  LoginThrottle(LoginThrottlePolicy policy,
                NowFunction now,
                SleepFunction sleep);

  // Records the start of an attempt by `keys` and returns how to treat it. A
  // locked-out attempt is not counted again, so hammering a locked address
  // does not extend its lockout.
  [[nodiscard]] Admission BeginAttempt(const Keys& keys);

  // Forgives every failure counted against `keys`, the attempt just begun
  // included.
  void RecordSuccess(const Keys& keys);

  // Waits `delay` using the sleep function given at construction.
  [[nodiscard]] Awaitable<void> Sleep(Clock::duration delay) const;

  [[nodiscard]] const LoginThrottlePolicy& policy() const { return policy_; }
  [[nodiscard]] std::size_t tracked_key_count() const { return keys_.size(); }

 private:
  struct KeyState {
    std::size_t failures = 0;
    TimePoint last_attempt;
    TimePoint locked_until;
  };

  // Returns the live state for `key`, first resetting one idle past
  // `forget_after`. Creates it (making room if needed) when absent.
  KeyState& Touch(const std::string& key, TimePoint now);
  [[nodiscard]] Clock::duration DelayFor(std::size_t failures) const;
  void MakeRoom(TimePoint now);

  LoginThrottlePolicy policy_;
  NowFunction now_;
  SleepFunction sleep_;
  std::unordered_map<std::string, KeyState> keys_;
};

// The address part of a transport peer string ("address:port", IPv6 bracketed
// as "[::1]:2000") — the port changes with every connection, so a throttle
// keyed on the whole string would never see a repeat. Returns `peer` unchanged
// when it carries no port.
[[nodiscard]] std::string LoginThrottlePeerAddress(std::string_view peer);
