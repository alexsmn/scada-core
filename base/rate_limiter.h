#pragma once

#include <chrono>

class RateLimiter {
 public:
  explicit RateLimiter(int max_rate)
      : max_rate_{max_rate}, max_burst_{max_rate}, tokens_{max_rate} {}

  RateLimiter(int max_rate, int max_burst)
      : max_rate_{max_rate}, max_burst_{max_burst}, tokens_{max_burst} {}

  // Returns true if the request is allowed.
  bool Request();

 private:
  const int max_rate_;
  const int max_burst_;

  int tokens_ = 0;
  std::chrono::steady_clock::time_point last_time_ =
      std::chrono::steady_clock::now();
};

inline bool RateLimiter::Request() {
  auto now = std::chrono::steady_clock::now();
  int elapsed_sec = static_cast<int>(
      std::chrono::duration_cast<std::chrono::seconds>(now - last_time_)
          .count());
  last_time_ = now;

  tokens_ += elapsed_sec * max_rate_;
  if (tokens_ > max_burst_)
    tokens_ = max_burst_;

  if (tokens_ == 0)
    return false;

  --tokens_;
  return true;
}
