#pragma once

#include "scada/session_service.h"

template <class Proxy>
class SessionProxyNotifier {
 public:
  SessionProxyNotifier(Proxy& proxy, scada::SessionService& session_service)
      : proxy_{proxy},
        session_state_changed_connection_{
            session_service.SubscribeSessionStateChanged(
                [&proxy](bool connected, const scada::Status& status) {
                  if (connected)
                    proxy.OnChannelOpened();
                  else
                    proxy.OnChannelClosed();
                })} {
    if (session_service.IsConnected())
      proxy_.OnChannelOpened();
  }

 private:
  Proxy& proxy_;

  const boost::signals2::scoped_connection session_state_changed_connection_;
};
