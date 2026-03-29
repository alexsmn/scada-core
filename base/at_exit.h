#pragma once

#include <functional>
#include <mutex>
#include <stack>

namespace base {

class AtExitManager {
 public:
  AtExitManager() { s_current = this; }

  ~AtExitManager() {
    while (!callbacks_.empty()) {
      callbacks_.top()();
      callbacks_.pop();
    }
    if (s_current == this)
      s_current = nullptr;
  }

  AtExitManager(const AtExitManager&) = delete;
  AtExitManager& operator=(const AtExitManager&) = delete;

  static void RegisterCallback(std::function<void()> callback) {
    if (s_current)
      s_current->callbacks_.push(std::move(callback));
  }

 private:
  std::stack<std::function<void()>> callbacks_;
  static inline AtExitManager* s_current = nullptr;
};

}  // namespace base
