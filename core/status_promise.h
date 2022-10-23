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

template <class Callback>
inline auto BindCallback(Callback&& callback) {
  return [callback = std::forward<Callback>(callback)](
             StatusCode status_code) mutable {
    return IsGood(status_code) ? callback()
                               : make_resolved_promise(status_code);
  };
}

template <class Callback>
inline auto BindError(Callback&& callback) {
  return [callback = std::forward<Callback>(callback)](
             StatusCode status_code) mutable {
    if (scada::IsBad(status_code))
      callback(status_code);
    return status_code;
  };
}

inline promise<> MakeStatusPromise(scada::StatusCode status_code) {
  return IsGood(status_code)
             ? make_resolved_promise()
             : make_rejected_promise(StatusException{status_code});
}

}  // namespace scada
