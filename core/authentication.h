#pragma once

#include "base/promise.h"
#include "base/struct_writer.h"
#include "core/localized_text.h"
#include "core/node_id.h"
#include "core/status.h"

#include <functional>

namespace scada {

struct AuthenticationResult {
  scada::NodeId user_id;
  unsigned user_rights = 0;
  bool multi_sessions = false;
};

using AuthenticationCallback =
    std::function<void(const AuthenticationResult& result)>;

// TODO: Merge into `SessionService`.
using Authenticator = std::function<promise<AuthenticationResult>(
    const scada::LocalizedText& user_name,
    const scada::LocalizedText& password)>;

inline std::ostream& operator<<(std::ostream& stream,
                                const AuthenticationResult& result) {
  StructWriter{stream}
      .AddField("user_id", result.user_id)
      .AddField("user_rights", result.user_rights)
      .AddField("multi_sessions", result.multi_sessions);
  return stream;
}

}  // namespace scada
