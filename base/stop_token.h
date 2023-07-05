#pragma once

#include <stop_token>

class scoped_stop_source {
 public:
  ~scoped_stop_source() {
    // Safe for repeated calls.
    stop_source_.request_stop();
  }

  std::stop_token get_token() const noexcept {
    return stop_source_.get_token();
  }

  void request_stop() noexcept { stop_source_.request_stop(); }

 private:
  std::stop_source stop_source_;
};
