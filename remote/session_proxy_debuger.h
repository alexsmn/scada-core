#pragma once

#include "scada/session_debugger.h"

#include <boost/signals2/signal.hpp>

class SessionProxyDebugger : public scada::SessionDebugger {
 public:
  void NotifyRequestEvent(const RequestEvent& request) {
    request_event_signal_(request);
  }

  // scada::SessionDebugger

  virtual boost::signals2::scoped_connection SubscribeRequestEvents(
      const RequestEventCallback& callback) override {
    return request_event_signal_.connect(callback);
  }

 private:
  boost::signals2::signal<void(const RequestEvent& event)>
      request_event_signal_;
};
