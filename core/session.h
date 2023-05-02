#pragma once

#include "core/session_service.h"

namespace scada {

class session {
 public:
  explicit session(SessionService& session_service)
      : session_service_{session_service} {}

 private:
  SessionService& session_service_;
};

}  // namespace scada