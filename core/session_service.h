#pragma once

#include "core/localized_text.h"
#include "core/node_id.h"
#include "core/privileges.h"
#include "core/status.h"
#include "core/status_callback.h"

#include <boost/signals2/connection.hpp>
#include <functional>
#include <string>

namespace base {
class TimeDelta;
}

namespace scada {

class SessionService {
 public:
  virtual ~SessionService() {}

  virtual void Connect(const std::string& connection_string,
                       const scada::LocalizedText& user_name,
                       const scada::LocalizedText& password,
                       bool allow_remote_logoff,
                       const StatusCallback& callback) = 0;

  virtual void Reconnect() = 0;

  virtual void Disconnect(const StatusCallback& callback) = 0;

  virtual bool IsConnected(base::TimeDelta* ping_delay = nullptr) const = 0;

  virtual NodeId GetUserId() const = 0;
  virtual bool HasPrivilege(Privilege privilege) const = 0;

  virtual std::string GetHostName() const = 0;

  virtual bool IsScada() const = 0;

  using SessionStateChangedCallback =
      std::function<void(bool connected, const Status& status)>;
  virtual boost::signals2::scoped_connection SubscribeSessionStateChanged(
      const SessionStateChangedCallback& callback) = 0;
};

}  // namespace scada
