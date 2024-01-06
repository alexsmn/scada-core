#pragma once

#include "base/promise.h"
#include "scada/status.h"

#include <promise.hpp/promise.hpp>

namespace scada {

// A promise that can only use `std::status_exception` on reject.
template <class T>
using status_promise = promise_hpp::promise<T>;

class status_exception : public std::exception {
 public:
  explicit status_exception(Status status) : status_{std::move(status)} {}

  const Status& status() const { return status_; }

  virtual const char* what() const noexcept override {
    return ToCString(status_.code());
  }

 private:
  const Status status_;
};

inline Status GetExceptionStatus(const std::exception_ptr& e) {
  try {
    std::rethrow_exception(e);
  } catch (const status_exception& e) {
    return e.status();
  } catch (...) {
    return StatusCode::Bad;
  }
}

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
  return promise.then(callback, [callback](const std::exception_ptr& e) {
    callback({.status = GetExceptionStatus(e)});
  });
}

// Properly assigns status callback to a status promise. The callback is invoked
// when status exception is triggered.
//
// WARNING: The current implementation requires a copyable callback since it has
// to be copied both to `.then` and to `.except` handlers.
template <class Callback>
inline status_promise<void> BindStatusCallback(status_promise<Status> promise,
                                               const Callback& callback) {
  // Explicit `callback` copy is required to make it mutable. Mutability is
  // needed e.g. when capturing promises.
  return promise
      .then([callback](const Status& status) {
        // Invoke callback with the actual `status`.
        // TODO: Avoid copy.
        callback(Status{status});
      })
      .except([callback](const std::exception_ptr& e) {
        callback(GetExceptionStatus(e));
      });
}

// WARNING: The current implementation requires a copyable callback since it has
// to be copied both to `.then` and to `.except` handlers.
template <class Callback>
inline status_promise<void> BindStatusCallback(status_promise<void> promise,
                                               const Callback& callback) {
  // Explicit `callback` copy is required to make it mutable. Mutability is
  // needed e.g. when capturing promises.
  return promise.then([callback]() mutable { callback(StatusCode::Good); })
      .except([callback](const std::exception_ptr& e) mutable {
        callback(GetExceptionStatus(e));
      });
}

template <class T>
inline auto MakeResolvedStatusPromise(T&& value) {
  return make_resolved_promise(std::forward<T>(value));
}

inline status_promise<void> MakeRejectedStatusPromise(Status status) {
  return make_rejected_promise<>(status_exception{std::move(status)});
}

template <class T>
inline status_promise<T> MakeRejectedStatusPromise(Status status) {
  return make_rejected_promise<T>(status_exception{std::move(status)});
}

inline status_promise<void> MakeCompletedStatusPromise(Status status) {
  return status ? make_resolved_promise()
                : make_rejected_promise<>(status_exception{std::move(status)});
}

inline status_promise<void> ToStatusPromise(promise<Status> promise) {
  return promise.then([](const Status& status) {
    return status.bad() ? MakeRejectedStatusPromise(status)
                        : make_resolved_promise();
  });
}

inline status_promise<void> ToStatusPromise(promise<StatusCode> promise) {
  return promise.then([](StatusCode status_code) {
    return IsBad(status_code) ? MakeRejectedStatusPromise(status_code)
                              : make_resolved_promise();
  });
}

inline promise<StatusCode> ToExplicitStatusCodePromise(
    status_promise<void> promise) {
  return promise.then(
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
inline void RejectStatusPromise(status_promise<T>& promise,
                                scada::Status bad_status) {
  assert(!bad_status);
  promise.reject(scada::status_exception{std::move(bad_status)});
}

inline void RejectStatusPromise(status_promise<void>& promise,
                                scada::Status bad_status) {
  assert(!bad_status);
  promise.reject(scada::status_exception{std::move(bad_status)});
}

inline void ResolveStatusPromise(status_promise<void>& promise,
                                 scada::Status status) {
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
