#pragma once

#include "base/awaitable.h"
#include "base/struct_writer.h"
#include "scada/localized_text.h"
#include "scada/node_id.h"
#include "scada/status.h"
#include "scada/status_or.h"

#include <functional>

namespace scada {

struct AuthenticationResult {
  scada::NodeId user_id;
  unsigned user_rights = 0;
  bool multi_sessions = false;
  // Custom (group) Roles granted to the session at activation by evaluating
  // the identity token against each role's identity mapping rules (OPC UA
  // Part 18 §4.4.1,
  // https://reference.opcfoundation.org/Core/Part18/v105/docs/4.4). Additive
  // to the well-known roles derived from `user_rights` (RolesForUser).
  std::vector<scada::NodeId> role_ids;
};

using AuthenticationCallback =
    std::function<void(const AuthenticationResult& result)>;

// TODO: Merge into `SessionService`.
using Authenticator =
    std::function<Awaitable<scada::StatusOr<AuthenticationResult>>(
        scada::LocalizedText user_name,
        scada::LocalizedText password)>;

using AsyncAuthenticator = Authenticator;

class CoroutineAuthenticator {
 public:
  virtual ~CoroutineAuthenticator() = default;

  virtual Awaitable<scada::StatusOr<AuthenticationResult>> Authenticate(
      scada::LocalizedText user_name,
      scada::LocalizedText password) = 0;
};

inline std::ostream& operator<<(std::ostream& stream,
                                const AuthenticationResult& result) {
  StructWriter{stream}
      .AddField("user_id", result.user_id)
      .AddField("user_rights", result.user_rights)
      .AddField("multi_sessions", result.multi_sessions)
      .AddField("role_count", result.role_ids.size());
  return stream;
}

}  // namespace scada
