#pragma once

#include "scada/localized_text.h"
#include "scada/node_id.h"
#include "scada/privileges.h"
#include "scada/status_promise.h"

#include <boost/signals2/connection.hpp>
#include <functional>
#include <string>

namespace base {
class TimeDelta;
}

namespace scada {

class SessionDebugger;

struct SessionConnectParams {
  // The host name can be followed by a colon and a port number. If empty, then
  // the `connection_string` is used.
  std::string host;
  // The connection string defines a `net::TransportString`. It's used if the
  // `host` is empty.
  std::string connection_string;
  LocalizedText user_name;
  LocalizedText password;
  bool allow_remote_logoff = false;
};

class SessionService {
 public:
  virtual ~SessionService() = default;

  virtual promise<> Connect(const SessionConnectParams& params) = 0;

  virtual promise<> Reconnect() = 0;

  virtual promise<> Disconnect() = 0;

  virtual bool IsConnected(base::TimeDelta* ping_delay = nullptr) const = 0;

  virtual NodeId GetUserId() const = 0;
  virtual bool HasPrivilege(Privilege privilege) const = 0;

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
