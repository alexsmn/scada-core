#pragma once

#include "scada/co_result.h"
#include "scada/session_service.h"

#include <gmock/gmock.h>

namespace scada {

class MockSessionService : public SessionService {
 public:
  MockSessionService() {
    using namespace testing;

    // A default-constructed `boost::asio::awaitable` -- gmock's fallback for a
    // return type it knows nothing about -- holds a null frame, and its
    // `await_ready()` still reports false, so co_awaiting it segfaults in
    // `await_suspend` inside the *awaiting* coroutine, naming no mock. Hand
    // back an already-complete awaitable instead, so an unstubbed call reads
    // as a session transition that needed no work.
    ON_CALL(*this, Connect(_))
        .WillByDefault(
            [](scada::SessionConnectParams) -> Awaitable<void> { co_return; });
    ON_CALL(*this, ConnectStatus(_))
        .WillByDefault([](scada::SessionConnectParams) -> CoStatus {
          co_return StatusCode::Good;
        });
    ON_CALL(*this, Reconnect()).WillByDefault([]() -> Awaitable<void> {
      co_return;
    });
    ON_CALL(*this, Disconnect()).WillByDefault([]() -> Awaitable<void> {
      co_return;
    });
  }

  MOCK_METHOD(Awaitable<void>,
              Connect,
              (scada::SessionConnectParams params),
              (override));

  MOCK_METHOD(CoStatus,
              ConnectStatus,
              (scada::SessionConnectParams params),
              (override));

  MOCK_METHOD(Awaitable<void>, Reconnect, (), (override));

  MOCK_METHOD(Awaitable<void>, Disconnect, (), (override));

  MOCK_METHOD(bool,
              IsConnected,
              (scada::Duration * ping_delay),
              (const override));

  MOCK_METHOD(NodeId, GetUserId, (), (const override));
  MOCK_METHOD(std::uint32_t, GetAccessRights, (), (const override));
  MOCK_METHOD(bool, IsAnonymous, (), (const override));

  MOCK_METHOD(std::string, GetHostName, (), (const override));

  MOCK_METHOD(bool, IsScada, (), (const override));

  MOCK_METHOD(boost::signals2::scoped_connection,
              SubscribeSessionStateChanged,
              (const SessionStateChangedCallback& callback),
              (override));

  MOCK_METHOD(SessionDebugger*, GetSessionDebugger, (), (override));
};

}  // namespace scada
