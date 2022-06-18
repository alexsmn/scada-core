#pragma once

#include <memory>

namespace internal {

template <class C, class T>
struct CancelationWrapper {
  template <class... Args>
  void operator()(Args&&... args) const {
    auto ref = cancelation_.lock();
    if (ref)
      std::move(task_)(std::forward<Args>(args)...);
  }

  const std::weak_ptr<C> cancelation_;
  T task_;
};

}  // namespace internal

template <class C, class T>
inline auto BindCancelation(std::weak_ptr<C> cancelation, T&& task) {
  return internal::CancelationWrapper<C, T>{std::move(cancelation),
                                            std::forward<T>(task)};
}

class Cancelation {
 public:
  template <class T>
  auto Bind(T&& task) const {
    return BindCancelation(std::weak_ptr<bool>{cancelation_},
                           std::forward<T>(task));
  }

  void Cancel() { cancelation_ = std::make_shared<bool>(); }

 private:
  std::shared_ptr<bool> cancelation_ = std::make_shared<bool>();
};
