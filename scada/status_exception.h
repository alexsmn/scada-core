#pragma once

#include "scada/status.h"

#include <exception>

namespace scada {

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

inline Status GetExceptionStatus(std::exception_ptr e) {
  try {
    std::rethrow_exception(e);
  } catch (const status_exception& e) {
    return e.status();
  } catch (...) {
    return StatusCode::Bad;
  }
}

}  // namespace scada