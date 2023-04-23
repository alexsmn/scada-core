#pragma once

#include "base/promise.h"
#include "core/status.h"

namespace scada {

class StatusException : public std::exception {
 public:
  explicit StatusException(scada::Status status) : status_{std::move(status)} {}

  const scada::Status& status() const { return status_; }

  virtual const char* what() const noexcept override {
    return ToCString(status_.code());
  }

 private:
  const scada::Status status_;
};

// WARNING: The current implementation requires a copyable callback since it has
// to be copied both to `.then` and `.except` handlers.
template <class Callback>
inline auto BindStatusCallback(promise<Status>& promise,
                               const Callback& callback) {
  return promise
      .then([callback](scada::Status& status) {
        callback(status);
        return status;
      })
      .except([callback](const StatusException& e) { callback(e.status()); });
}

inline promise<> ToStatusPromise(promise<Status> promise) {
  return promise.then([](const Status& status) {
    return status.bad() ? make_rejected_promise(StatusException{status.code()})
                        : make_resolved_promise();
  });
}

inline promise<> MakeRejectedStatusPromise(scada::Status status) {
  return make_rejected_promise<>(StatusException{std::move(status)});
}

template <class T>
inline promise<T> MakeRejectedStatusPromise(scada::Status status) {
  return make_rejected_promise<T>(StatusException{std::move(status)});
}

}  // namespace scada
