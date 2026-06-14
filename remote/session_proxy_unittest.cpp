#include "remote/session_proxy.h"

#include "base/test/asio_test_environment.h"
#include "base/test/network_test_environment.h"
#include "remote/remote_session_manager.h"
#include "remote/session_stub.h"
#include "scada/authentication_adapters.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <optional>

using namespace testing;

namespace {

class SessionManagerObserver final : public RemoteSessionManager::Observer {
 public:
  void OnSessionOpened(SessionStub& session) override {
    opened_user_ids.push_back(session.service_context().user_id());
  }

  void OnSessionClosed(SessionStub& session) override {
    closed_user_ids.push_back(session.service_context().user_id());
  }

  std::vector<scada::NodeId> opened_user_ids;
  std::vector<scada::NodeId> closed_user_ids;
};

class SessionProxyTest : public Test {
 public:
  void SetUp() override { CreateSessionManager(); }

  void TearDown() override {
    session_manager_.reset();
    asio_env_.Poll();
  }

 protected:
  void CreateSessionManager() {
    session_manager_ =
        std::make_unique<RemoteSessionManager>(RemoteSessionManagerContext{
            .executor_ = asio_env_.any_executor_factory(),
            .authenticator_ = scada::MakeCoroutineAuthenticator(
                [this](scada::LocalizedText, scada::LocalizedText)
                    -> Awaitable<scada::StatusOr<scada::AuthenticationResult>> {
                  if (auth_failure_status_) {
                    co_return *auth_failure_status_;
                  }
                  co_return scada::AuthenticationResult{.user_id = kUserId};
                }),
            .transport_factory_ = asio_env_.transport_factory,
            .endpoints_ = {transport::TransportString{
                network_env_.server_transport_string}}});

    asio_env_.Wait(session_manager_->InitAsync());
  }

  scada::SessionConnectParams GetConnectParams() const {
    return {.connection_string = network_env_.client_transport_string,
            .user_name = kUserName,
            .password = kPassword};
  }

  AsioTestEnvironment asio_env_;
  NetworkTestEnvironment network_env_;
  std::optional<scada::StatusCode> auth_failure_status_;
  std::unique_ptr<RemoteSessionManager> session_manager_;

  inline static const scada::LocalizedText kUserName{u"username"};
  inline static const scada::LocalizedText kPassword{u"password"};
  inline static const scada::NodeId kUserId{1, 1};
};

TEST_F(SessionProxyTest, ConnectAndDisconnectAreAwaitable) {
  SessionProxy session{{.executor_ = asio_env_.any_executor_factory(),
                        .transport_factory_ = asio_env_.transport_factory}};

  asio_env_.Wait(session.Connect(GetConnectParams()));
  EXPECT_TRUE(session.IsConnected(nullptr));

  asio_env_.Wait(session.Disconnect());
  EXPECT_FALSE(session.IsConnected(nullptr));
}

TEST_F(SessionProxyTest, DestroyConnectedSessionClosesChildChannels) {
  {
    SessionProxy session{{.executor_ = asio_env_.any_executor_factory(),
                          .transport_factory_ = asio_env_.transport_factory}};

    asio_env_.Wait(session.Connect(GetConnectParams()));
    EXPECT_TRUE(session.IsConnected(nullptr));

    // Let the subscription proxy finish opening so destruction covers the
    // same child-channel state used by a live client shutdown.
    asio_env_.PumpFor(std::chrono::milliseconds{100});
  }

  asio_env_.Poll();
}

TEST_F(SessionProxyTest, ConnectStatusReturnsBadCredentials) {
  auth_failure_status_ = scada::StatusCode::Bad_WrongLoginCredentials;

  SessionProxy session{{.executor_ = asio_env_.any_executor_factory(),
                        .transport_factory_ = asio_env_.transport_factory}};

  auto status = asio_env_.Wait(session.ConnectStatus(GetConnectParams()));
  EXPECT_EQ(status.code(), scada::StatusCode::Bad_WrongLoginCredentials);
}

TEST_F(SessionProxyTest, ReconnectIsAwaitable) {
  auto params = GetConnectParams();
  params.allow_remote_logoff = true;

  SessionProxy session{{.executor_ = asio_env_.any_executor_factory(),
                        .transport_factory_ = asio_env_.transport_factory}};

  asio_env_.Wait(session.Connect(params));
  asio_env_.Wait(session.Reconnect());
  EXPECT_TRUE(session.IsConnected(nullptr));

  asio_env_.Wait(session.Disconnect());
}

TEST_F(SessionProxyTest, ManagerObserverNotifiedOnOpenAndClose) {
  SessionManagerObserver observer;
  session_manager_->AddObserver(observer);

  SessionProxy session{{.executor_ = asio_env_.any_executor_factory(),
                        .transport_factory_ = asio_env_.transport_factory}};

  asio_env_.Wait(session.Connect(GetConnectParams()));
  EXPECT_THAT(observer.opened_user_ids, ElementsAre(kUserId));
  EXPECT_THAT(observer.closed_user_ids, IsEmpty());

  asio_env_.Wait(session.Disconnect());
  EXPECT_THAT(observer.closed_user_ids, ElementsAre(kUserId));

  session_manager_->RemoveObserver(observer);
}

}  // namespace
