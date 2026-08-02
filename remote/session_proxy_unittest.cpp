#include "remote/session_proxy.h"

#include "base/test/asio_test_environment.h"
#include "base/test/network_test_environment.h"
#include "remote/remote_session_manager.h"
#include "remote/session_stub.h"
#include "scada/authentication_adapters.h"
#include "scada/co_result.h"

#include <boost/signals2/connection.hpp>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <chrono>
#include <optional>
#include <tuple>

using namespace testing;

namespace {

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
                    -> scada::CoStatusOr<scada::AuthenticationResult> {
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
  std::vector<scada::NodeId> opened_user_ids;
  std::vector<scada::NodeId> closed_user_ids;
  boost::signals2::scoped_connection opened_connection =
      session_manager_->SubscribeSessionOpened([&](SessionStub& session) {
        opened_user_ids.push_back(session.service_context().user_id());
      });
  boost::signals2::scoped_connection closed_connection =
      session_manager_->SubscribeSessionClosed([&](SessionStub& session) {
        closed_user_ids.push_back(session.service_context().user_id());
      });

  SessionProxy session{{.executor_ = asio_env_.any_executor_factory(),
                        .transport_factory_ = asio_env_.transport_factory}};

  asio_env_.Wait(session.Connect(GetConnectParams()));
  EXPECT_THAT(opened_user_ids, ElementsAre(kUserId));
  EXPECT_THAT(closed_user_ids, IsEmpty());

  asio_env_.Wait(session.Disconnect());
  EXPECT_THAT(closed_user_ids, ElementsAre(kUserId));
}

// Regression: both read loops (SessionProxy::Connect here, ServerConnection on
// the other end) used to resize their receive buffer to protocol::kMaxMessageSize
// before every read and back down to the message length after it, so each
// message cost a 16 MiB value-initialization plus a 16 MiB destroy. On the Qt
// client that runs on the thread which also drives the UI and every coroutine
// continuation, and it pinned that thread at 100% CPU: the loop could not keep
// up with a steady notification stream, the backlog grew without bound, and each
// service response came back later than the last, so lazily-expanding views
// (the object tree) never finished loading no matter how long they waited.
//
// This is a cost, not a behaviour, so the assertion is a time budget. The margin
// is what keeps it non-fragile rather than the precision — measured on one macOS
// arm64 Debug build, the two are 159x apart:
//
//   pre-fix   33.5 s   (~168 ms per round trip: two 16 MiB resizes each side)
//   fixed      0.21 s  (~1 ms per round trip)
//
// The 5 s budget therefore sits ~24x above the fixed cost and ~6.7x below the
// regressed one, so neither a loaded machine nor a faster one flips the verdict.
TEST_F(SessionProxyTest, RoundTripsDoNotPayPerMessageBufferCost) {
  SessionProxy session{{.executor_ = asio_env_.any_executor_factory(),
                        .transport_factory_ = asio_env_.transport_factory}};

  asio_env_.Wait(session.Connect(GetConnectParams()));

  // The manager is built without an attribute service, so the stub answers each
  // Read with Bad immediately. The status is irrelevant — what the test
  // exercises is a full message round trip through both read loops.
  constexpr int kRoundTrips = 200;
  const auto start = std::chrono::steady_clock::now();
  for (int i = 0; i < kRoundTrips; ++i) {
    std::ignore = asio_env_.Wait(session.Read(
        scada::ServiceContext{},
        {scada::ReadValueId{scada::NodeId{1, 1}, scada::AttributeId::Value}}));
  }
  const auto elapsed = std::chrono::steady_clock::now() - start;

  EXPECT_LT(elapsed, std::chrono::seconds{5})
      << "200 round trips took "
      << std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count()
      << " ms; the read loops are paying a per-message cost proportional to "
         "protocol::kMaxMessageSize again";

  asio_env_.Wait(session.Disconnect());
}

}  // namespace
