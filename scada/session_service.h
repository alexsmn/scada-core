#pragma once

#include "base/awaitable.h"
#include "base/time/time.h"
#include "scada/access_rights.h"
#include "scada/authorization.h"
#include "scada/co_result.h"
#include "scada/date_time.h"
#include "scada/localized_text.h"
#include "scada/node_id.h"
#include "scada/status.h"

#include <boost/signals2/connection.hpp>
#include <functional>
#include <string>
#include <utility>

namespace scada {

class SessionDebugger;
class Status;

// Endpoint security selection for a session. Transport-neutral so the generic
// SessionService contract stays backend-agnostic; the OPC UA backend maps it to
// concrete SecurityPolicy / MessageSecurityMode choices. The defaults
// (mode=None, empty paths) preserve the legacy direct-connect behaviour with no
// discovery, so non-OPC-UA backends and existing callers are unaffected.
struct SessionSecuritySettings {
  enum class Mode {
    // No discovery; connect directly with no security (legacy behaviour).
    None,
    // Run discovery (GetEndpoints) and pick the most secure endpoint the
    // client supports.
    Auto,
    // Run discovery and require an encrypted (SignAndEncrypt) endpoint.
    SignAndEncrypt,
  };
  Mode mode = Mode::None;
  // Optional explicit SecurityPolicy URI to require, narrowing Auto selection.
  std::string required_policy_uri;
  // PEM file paths for the client application instance certificate and its
  // private key. Required when `mode` selects a secured endpoint.
  std::string client_certificate_path;
  std::string client_private_key_path;
  // Trust store used to authenticate the SERVER's application instance
  // certificate, mirroring the server-side store that authenticates clients
  // (`OpcUaModuleParams`). All empty means the server is not authenticated:
  // its certificate arrives inside the discovered EndpointDescription, so a
  // man in the middle can substitute its own and the channel is encrypted to
  // the attacker. See docs/server/opcua-client-security.md.
  std::string trusted_certificates_dir;
  std::string issuer_certificates_dir;
  std::string crl_dir;
  std::string rejected_certificates_dir;

  // True once any trust-store directory is configured; see the OPC UA-side
  // mirror of this struct for why selection tightens when it is.
  [[nodiscard]] bool has_trust_store() const {
    return !trusted_certificates_dir.empty() ||
           !issuer_certificates_dir.empty();
  }
};

struct SessionConnectParams {
  // The host name can be followed by a colon and a port number. If empty, then
  // the `connection_string` is used.
  std::string host;
  // The connection string defines a `transport::TransportString`. It's used if
  // the `host` is empty.
  std::string connection_string;
  LocalizedText user_name;
  LocalizedText password;
  bool allow_remote_logoff = false;
  // How to negotiate endpoint security. Defaults to the legacy unsecured path.
  SessionSecuritySettings security;
};

class SessionService {
 public:
  virtual ~SessionService() = default;

  virtual Awaitable<void> Connect(SessionConnectParams params) = 0;

  virtual CoStatus ConnectStatus(SessionConnectParams params) {
    co_await Connect(std::move(params));
    co_return StatusCode::Good;
  }

  virtual Awaitable<void> Reconnect() = 0;

  virtual Awaitable<void> Disconnect() = 0;

  virtual bool IsConnected(scada::Duration* ping_delay = nullptr) const = 0;

  virtual NodeId GetUserId() const = 0;

  // The session account's coarse access-rights bitmask (bits from
  // scada::AccessRight), as granted at session activation. Zero grants nothing
  // beyond the Observer baseline.
  virtual std::uint32_t GetAccessRights() const = 0;

  // True when the session has no authenticated user (an OPC UA Anonymous
  // identity token), which confines it to the Anonymous role. Sessions that
  // are authenticated by construction — in-process, test and vendor-bridge
  // sessions — keep the default.
  virtual bool IsAnonymous() const { return false; }

  // True when the session holds the coarse account right `right`. Use this only
  // for account-tier questions ("is this an administrator?"); gate a concrete
  // operation with HasPermission() so the client asks exactly what the server
  // enforces.
  bool HasAccessRight(AccessRight right) const {
    return scada::HasAccessRight(GetAccessRights(), right);
  }

  // The session's effective OPC UA permissions (Part 3 §8.55 PermissionType),
  // derived from its access rights through the well-known role map.
  // https://reference.opcfoundation.org/Core/Part3/v105/docs/8.55
  Permission GetPermissions() const {
    return PermissionsForUser(GetAccessRights(), IsAnonymous());
  }

  // True when the session's effective permissions include every bit of
  // `permission`. This is the same derivation the server applies when it
  // authorizes the corresponding service call, so a client-side gate and the
  // server's enforcement cannot disagree.
  bool HasPermission(Permission permission) const {
    return IsPermitted(GetAccessRights(), IsAnonymous(), permission);
  }

  virtual std::string GetHostName() const = 0;

  // TODO: Describe and explain the need.
  virtual bool IsScada() const = 0;

  using SessionStateChangedCallback =
      std::function<void(bool connected, const Status& status)>;

  virtual boost::signals2::scoped_connection SubscribeSessionStateChanged(
      const SessionStateChangedCallback& callback) = 0;

  virtual SessionDebugger* GetSessionDebugger() = 0;
};

}  // namespace scada
