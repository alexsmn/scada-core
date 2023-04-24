#pragma once

#include "base/promise.h"
#include "core/status.h"

namespace scada {

class StatusException : public std::exception {
 public:
  explicit StatusException(Status status) : status_{std::move(status)} {}

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
  } catch (const StatusException& e) {
    return e.status();
  } catch (...) {
    return StatusCode::Bad;
  }
}

// Properly assigns status callback to a status promise. The callback is invoked
// when status exception is triggered.
//
// WARNING: The current implementation requires a copyable callback since it has
// to be copied both to `.then` and
// `.except` handlers.
template <class Callback>
inline promise<> BindStatusCallback(promise<Status> promise,
                                    const Callback& callback) {
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

template <class Callback>
inline promise<> BindStatusCallback(promise<> promise,
                                    const Callback& callback) {
  return promise.then([callback] { callback(StatusCode::Good); })
      .except([callback](const std::exception_ptr& e) {
        callback(GetExceptionStatus(e));
      });
}

inline promise<> ToStatusPromise(promise<Status> promise) {
  return promise.then([](const Status& status) {
    return status.bad() ? make_rejected_promise(StatusException{status.code()})
                        : make_resolved_promise();
  });
}

inline promise<> MakeRejectedStatusPromise(Status status) {
  return make_rejected_promise<>(StatusException{std::move(status)});
}

template <class T>
inline promise<T> MakeRejectedStatusPromise(Status status) {
  return make_rejected_promise<T>(StatusException{std::move(status)});
}

}  // namespace scada
