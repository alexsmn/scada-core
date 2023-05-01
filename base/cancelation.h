#pragma once

#include <memory>

namespace internal {

template <class C, class T>
struct CancelationWrapper {
  template <class... Args>
  void operator()(Args&&... args) const {
    if (auto ref = cancelation_.lock())
      task_(std::forward<Args>(args)...);
  }

  template <class... Args>
  void operator()(Args&&... args) {
    if (auto ref = cancelation_.lock())
      task_(std::forward<Args>(args)...);
  }

  const std::weak_ptr<C> cancelation_;
  T task_;
};

template <class C, class F, class CF>
struct CancelationFuncWrapper {
  template <class... Args>
  auto operator()(Args&&... args) const {
    if (auto ref = cancelation_.lock())
      return func_(std::forward<Args>(args)...);
    else
      return canceled_func_();
  }

  template <class... Args>
  auto operator()(Args&&... args) {
    if (auto ref = cancelation_.lock())
      return func_(std::forward<Args>(args)...);
    else
      return canceled_func_();
  }

  const std::weak_ptr<C> cancelation_;
  F func_;
  CF canceled_func_;
};

}  // namespace internal

template <class C, class T>
inline auto BindCancelation(std::weak_ptr<C> cancelation, T&& task) {
  return internal::CancelationWrapper<C, T>{std::move(cancelation),
                                            std::forward<T>(task)};
}

template <class C, class F, class CF>
inline auto BindCancelationFunc(std::weak_ptr<C> cancelation,
                                F&& func,
                                CF&& canceled_func) {
  return internal::CancelationFuncWrapper<C, F, CF>{
      std::move(cancelation), std::forward<F>(func),
      std::forward<CF>(canceled_func)};
}

class Cancelation;

class CancelationRef {
 public:
  explicit CancelationRef(const Cancelation& cancelation);

  bool canceled() const { return cancelation_.expired(); }

 private:
  std::weak_ptr<bool> cancelation_;
};

class Cancelation {
 public:
  CancelationRef ref() const { return CancelationRef{*this}; }

  template <class T>
  auto Bind(T&& task) const {
    return BindCancelation(std::weak_ptr<bool>{cancelation_},
                           std::forward<T>(task));
  }

  void Cancel() { cancelation_ = std::make_shared<bool>(); }

 private:
  std::shared_ptr<bool> cancelation_ = std::make_shared<bool>();

  friend class CancelationRef;
};

inline CancelationRef::CancelationRef(const Cancelation& cancelation)
    : cancelation_{std::weak_ptr<bool>{cancelation.cancelation_}} {}
