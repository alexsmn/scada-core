#include "remote/session_proxy.h"

#include "base/test/asio_test_environment.h"
#include "base/test/network_test_environment.h"
#include "base/test/test_executor.h"
#include "remote/remote_session_manager.h"
#include "scada/authentication_mock.h"
#include "scada/status_promise.h"

#include <gmock/gmock.h>

using namespace testing;

class SessionProxyTest : public Test {
 public:
  void SetUp() override {
    ON_CALL(authenticator_, Call(_, _))
        .WillByDefault(Return(make_resolved_promise(
            scada::AuthenticationResult{.user_id = kUserId})));

    asio_env_.Wait(session_manager_.Init());
  }

 protected:
  scada::SessionConnectParams GetConnectParams() const {
    return {.connection_string = network_env_.client_transport_string,
            .user_name = kUserName,
            .password = kPassword};
  }

  AsioTestEnvironment asio_env_;
  NetworkTestEnvironment network_env_;

  NiceMock<scada::MockAuthenticator> authenticator_;

  RemoteSessionManager session_manager_{
      {.executor_ = std::make_shared<TestExecutor>(),
       .services_ = {},
       .authenticator_ = authenticator_.AsStdFunction(),
       .transport_factory_ = asio_env_.transport_factory,
       .endpoints_ = {transport::TransportString{
           network_env_.server_transport_string}}}};

  inline static const scada::LocalizedText kUserName{u"username"};
  inline static const scada::LocalizedText kPassword{u"password"};
  inline static const scada::NodeId kUserId{1, 1};
};

// Verifies that SessionProxy::Connect() resolves the connect_promise_.
// This exercises the fix where connect_promise_ is initialized in Reconnect()
// before co_spawn, ensuring the promise returned by Reconnect() is the same one
// resolved by ForwardConnectResult() inside the Connect() coroutine.
TEST_F(SessionProxyTest, Connect) {
  SessionProxy session{{.executor_ = asio_env_.executor,
                        .transport_factory_ = asio_env_.transport_factory}};

  asio_env_.Wait(session.Connect(GetConnectParams()));

  EXPECT_TRUE(session.IsConnected(nullptr));

  asio_env_.Wait(session.Disconnect());
}

// Verifies that Connect() with wrong credentials rejects the promise.
TEST_F(SessionProxyTest, Connect_BadPassword) {
  EXPECT_CALL(authenticator_, Call(kUserName, kPassword))
      .WillOnce(
          Return(scada::MakeRejectedStatusPromise<scada::AuthenticationResult>(
              scada::StatusCode::Bad_WrongLoginCredentials)));

  SessionProxy session{{.executor_ = asio_env_.executor,
                        .transport_factory_ = asio_env_.transport_factory}};

  EXPECT_THROW(asio_env_.Wait(session.Connect(GetConnectParams())),
               scada::status_exception);
}

// Verifies that Connect() followed by Disconnect() completes cleanly.
TEST_F(SessionProxyTest, Connect_Disconnect) {
  SessionProxy session{{.executor_ = asio_env_.executor,
                        .transport_factory_ = asio_env_.transport_factory}};

  asio_env_.Wait(session.Connect(GetConnectParams()));
  EXPECT_TRUE(session.IsConnected(nullptr));

  asio_env_.Wait(session.Disconnect());
  EXPECT_FALSE(session.IsConnected(nullptr));
}

// Verifies that Reconnect() resolves the connect_promise_ after
// disconnect+reconnect cycle.
TEST_F(SessionProxyTest, Reconnect) {
  auto params = GetConnectParams();
  params.allow_remote_logoff = true;

  SessionProxy session{{.executor_ = asio_env_.executor,
                        .transport_factory_ = asio_env_.transport_factory}};

  asio_env_.Wait(session.Connect(params));
  EXPECT_TRUE(session.IsConnected(nullptr));

  asio_env_.Wait(session.Reconnect());
  EXPECT_TRUE(session.IsConnected(nullptr));

  asio_env_.Wait(session.Disconnect());
}
