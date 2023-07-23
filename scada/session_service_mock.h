#pragma once

#include "scada/session_service.h"

#include <gmock/gmock.h>

namespace scada {

class MockSessionService : public SessionService {
 public:
  MOCK_METHOD(promise<>,
              Connect,
              (const scada::SessionConnectParams& params),
              (override));

  MOCK_METHOD(promise<>, Reconnect, (), (override));

  MOCK_METHOD(promise<>, Disconnect, (), (override));

  MOCK_METHOD(bool,
              IsConnected,
              (base::TimeDelta * ping_delay),
              (const override));

  MOCK_METHOD(NodeId, GetUserId, (), (const override));
  MOCK_METHOD(bool, HasPrivilege, (Privilege privilege), (const override));

  MOCK_METHOD(std::string, GetHostName, (), (const override));

  MOCK_METHOD(bool, IsScada, (), (const override));

  MOCK_METHOD(boost::signals2::scoped_connection,
              SubscribeSessionStateChanged,
              (const SessionStateChangedCallback& callback),
              (override));

  MOCK_METHOD(SessionDebugger*, GetSessionDebugger, (), (override));
};

}  // namespace scada