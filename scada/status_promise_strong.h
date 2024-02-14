#pragma once

#include "base/promise.h"
#include "scada/status_exception.h"

#include <promise.hpp/promise.hpp>

namespace scada {

template <class T>
class status_promise;

template <class T>
struct is_status_promise : std::false_type {};

template <class T>
struct is_status_promise<status_promise<T>> : std::true_type {};

template <class T>
constexpr bool is_status_promise_v = is_status_promise<T>::value;

template <class R, class T>
struct resolver_result {
  using type = std::invoke_result_t<R, T>;
};

template <class R>
struct resolver_result<R, void> {
  using type = std::invoke_result_t<R>;
};

// Like `invoke_result_t<R, T>`, but works well with void `T`.
template <class R, class T>
using resolver_result_t = typename resolver_result<R, T>::type;

// A promise that can only use `std::status_exception` on reject.
template <class T>
class status_promise final {
 public:
  static_assert(!std::is_same_v<T, Status>,
                "scada::status_promise type cannot be a status");
  static_assert(!std::is_same_v<T, StatusCode>,
                "scada::status_promise type cannot be a status code");

  using value_type = T;

  status_promise() = default;
  explicit status_promise(promise_hpp::promise<T> promise)
      : promise_{std::move(promise)} {}

  const promise_hpp::promise<T>& promise() const { return promise_; }
  promise_hpp::promise<T>& promise() { return promise_; }

  template <class U, class = std::enable_if_t<!std::is_void_v<T>, bool>>
  void resolve(U&& value) {
    promise_.resolve(std::forward<U>(value));
  }

  template <class = std::enable_if_t<std::is_void_v<T>, bool>>
  void resolve() {
    promise_.resolve();
  }

  void reject(status_exception&& e) { promise_.reject(std::move(e)); }

  template <class R>
  auto then(R&& resolver) {
    return scada::status_promise{
        promise_.then(UnwrapResolver(std::forward<R>(resolver)))};
  }

  template <class R, class E>
  auto then(R&& resolver, E&& rejector) {
    return scada::status_promise{
        promise_.then(UnwrapResolver(std::forward<R>(resolver)),
                      WrapRejector(std::forward<E>(rejector)))};
  }

 private:
  template <class R>
  auto UnwrapResolver(R&& resolver) {
    using Result = resolver_result_t<R, T>;
    if constexpr (is_status_promise_v<Result>) {
      if constexpr (std::is_void_v<T>) {
        return [resolver = std::forward<R>(resolver)]() mutable {
          return resolver().promise();
        };
      } else {
        return [resolver = std::forward<R>(resolver)](const T& value) mutable {
          return resolver(value).promise();
        };
      }
    } else {
      static_assert(
          !promise_hpp::is_promise_v<Result>,
          "scada::status_promise resolver cannot return a raw promise");
      return resolver;
    }
  }

  template <class E>
  auto WrapRejector(E&& rejector) {
    return
        [rejector = std::forward<E>(rejector)](std::exception_ptr e) mutable {
          return rejector(GetExceptionStatus(e));
        };
  }

  promise_hpp::promise<T> promise_;
};

// Properly assigns status callback to a status promise. The callback is invoked
// when status exception is triggered.
//
// WARNING: The current implementation requires a copyable callback since it has
// to be copied both to `.then` and to `.except` handlers.
template <class T, class Callback>
inline status_promise<void> BindStatusCallback(status_promise<T> promise,
                                               const Callback& callback) {
  // Explicit `callback` copy is required to make it mutable. Mutability is
  // needed e.g. when capturing promises.
  return promise.then(callback, [callback](Status status) {
    callback({.status = std::move(status)});
  });
}

// WARNING: The current implementation requires a copyable callback since it has
// to be copied both to `.then` and to `.except` handlers.
template <class Callback>
inline status_promise<void> BindStatusCallback(status_promise<void> promise,
                                               const Callback& callback) {
  // Explicit `callback` copy is required to make it mutable. Mutability is
  // needed e.g. when capturing promises.
  return promise.then([callback]() mutable { callback(StatusCode::Good); },
                      callback);
}

inline status_promise<void> MakeResolvedStatusPromise() {
  return status_promise{make_resolved_promise()};
}

template <class T>
inline auto MakeResolvedStatusPromise(T&& value) {
  return status_promise{make_resolved_promise(std::forward<T>(value))};
}

inline status_promise<void> MakeRejectedStatusPromise(Status status) {
  return status_promise{
      make_rejected_promise<>(status_exception{std::move(status)})};
}

template <class T>
inline status_promise<T> MakeRejectedStatusPromise(Status status) {
  return status_promise{
      make_rejected_promise<T>(status_exception{std::move(status)})};
}

inline status_promise<void> MakeCompletedStatusPromise(Status status) {
  return status_promise{
      status ? make_resolved_promise()
             : make_rejected_promise<>(status_exception{std::move(status)})};
}

inline [[nodiscard]] status_promise<void> ToStatusPromise(
    promise<Status> promise) {
  return status_promise{promise.then([](const Status& status) {
    return status.bad() ? MakeRejectedStatusPromise(status).promise()
                        : make_resolved_promise();
  })};
}

inline status_promise<void> ToStatusPromise(promise<StatusCode> promise) {
  return status_promise{promise.then([](StatusCode status_code) {
    return IsBad(status_code) ? MakeRejectedStatusPromise(status_code).promise()
                              : make_resolved_promise();
  })};
}

inline promise<StatusCode> ToExplicitStatusCodePromise(
    status_promise<void> promise) {
  return promise.promise().then(
      [] { return StatusCode::Good; },
      [](const std::exception_ptr& e) { return GetExceptionStatus(e).code(); });
}

// A callback to pass to a promise:
// Example:
//   status_promise<> promise;
//   session_proxy_.Connect("port=3000", u"username", u"password",
//     MakeStatusPromiseCallback(promise));
inline auto MakeStatusPromiseCallback(status_promise<void> promise) {
  return [promise = std::move(promise)](Status status) mutable {
    if (status.bad()) {
      promise.reject(status_exception{std::move(status)});
    } else {
      promise.resolve();
    }
  };
}

template <class T>
inline void RejectStatusPromise(status_promise<T>& promise, Status bad_status) {
  assert(!bad_status);
  promise.reject(scada::status_exception{std::move(bad_status)});
}

inline void RejectStatusPromise(status_promise<void>& promise,
                                Status bad_status) {
  assert(!bad_status);
  promise.reject(scada::status_exception{std::move(bad_status)});
}

inline void ResolveStatusPromise(status_promise<void>& promise, Status status) {
  if (status) {
    promise.resolve();
  } else {
    RejectStatusPromise(promise, std::move(status));
  }
}

template <class T>
inline auto MakeStatusPromiseCallback(status_promise<T> promise) {
  return [promise = std::move(promise)](T&& result) mutable {
    if (!result.status) {
      RejectStatusPromise(promise, std::move(result.status));
    } else {
      promise.resolve(std::move(result));
    }
  };
}

template <class T>
inline auto MakeStatusCodePromiseCallback(status_promise<T> promise) {
  return [promise = std::move(promise)](T&& result) mutable {
    if (IsBad(result.status_code)) {
      RejectStatusPromise(promise, result.status_code);
    } else {
      promise.resolve(std::move(result));
    }
  };
}

}  // namespace scada

template <class T>
void ForwardPromise(scada::status_promise<T> from,
                    scada::status_promise<T> to) {
  ForwardPromise(from.promise(), to.promise());
}
