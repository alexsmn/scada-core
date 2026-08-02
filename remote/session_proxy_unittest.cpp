#include "remote/session_proxy.h"

#include "base/async_completion.h"
#include "base/test/asio_test_environment.h"
#include "base/test/network_test_environment.h"
#include "remote/protocol.h"
#include "remote/protocol_message_transport.h"
#include "remote/remote_session_manager.h"
#include "remote/session_stub.h"
#include "scada/authentication_adapters.h"
#include "scada/co_result.h"

#include <boost/asio/this_coro.hpp>
#include <boost/signals2/connection.hpp>
#include <chrono>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <memory>
#include <optional>
#include <string>
#include <transport/transport_factory.h>
#include <transport/transport_string.h>
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
                  ++authentications_started_;
                  if (auth_gate_) {
                    co_await auth_gate_->Wait();
                  }
                  co_return scada::AuthenticationResult{.user_id = kUserId};
                }),
            .transport_factory_ = asio_env_.transport_factory,
            .endpoints_ = {transport::TransportString{
                network_env_.server_transport_string}}});

    asio_env_.Wait(session_manager_->InitAsync());
  }

  // Opens a raw framed connection, sends one CreateSession, and closes it
  // without waiting for the reply — a client killed with its logon in flight.
  void SendCreateSessionAndDropConnection() {
    const bool sent = asio_env_.Wait([this]() -> Awaitable<bool> {
      auto executor = co_await boost::asio::this_coro::executor;
      auto raw = asio_env_.transport_factory.CreateTransport(
          transport::TransportString{network_env_.client_transport_string},
          executor, transport::log_source{});
      if (!raw.ok()) {
        co_return false;
      }

      transport::any_transport transport{
          std::make_unique<ProtocolMessageTransport>(std::move(*raw))};
      if (co_await transport.open() != transport::OK) {
        co_return false;
      }

      protocol::Message message;
      auto& request = *message.add_requests();
      request.set_request_id(1);
      auto& create_session = *request.mutable_create_session();
      create_session.set_user_name_utf8("username");
      create_session.set_password_utf8("password");
      create_session.set_protocol_version_major(
          protocol::PROTOCOL_VERSION_MAJOR);
      create_session.set_protocol_version_minor(
          protocol::PROTOCOL_VERSION_MINOR);

      std::string payload;
      if (!message.AppendToString(&payload)) {
        co_return false;
      }
      auto written = co_await transport.write(payload);
      std::ignore = co_await transport.close();
      co_return written.ok() && *written == payload.size();
    }());
    ASSERT_TRUE(sent);
  }

  scada::SessionConnectParams GetConnectParams() const {
    return {.connection_string = network_env_.client_transport_string,
            .user_name = kUserName,
            .password = kPassword};
  }

  AsioTestEnvironment asio_env_;
  NetworkTestEnvironment network_env_;
  std::optional<scada::StatusCode> auth_failure_status_;
  // When set, authentication suspends until the test completes it — the window
  // in which a client can go away while its logon is still in flight.
  std::optional<scada::base::AsyncCompletion> auth_gate_;
  int authentications_started_ = 0;
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

// Regression: a client that vanishes without sending DeleteSession must not
// keep its account logged on. The manager refuses a second logon for a
// single-session account (`multi_sessions == false`, which is what the test
// authenticator returns), so a session left behind by a dropped connection
// locks the account's own owner out — and this manager has no session timeout
// to end the lockout.
//
// The ordinary drop is covered by the connection's read failing; this pins it
// so that path cannot regress silently.
TEST_F(SessionProxyTest, DroppedConnectionDoesNotKeepAccountLoggedOn) {
  {
    SessionProxy session{{.executor_ = asio_env_.any_executor_factory(),
                          .transport_factory_ = asio_env_.transport_factory}};
    asio_env_.Wait(session.Connect(GetConnectParams()));
    // Destroyed without Disconnect(): no DeleteSession is ever sent.
  }
  asio_env_.PumpFor(std::chrono::milliseconds{100});

  SessionProxy session{{.executor_ = asio_env_.any_executor_factory(),
                        .transport_factory_ = asio_env_.transport_factory}};
  auto status = asio_env_.Wait(session.ConnectStatus(GetConnectParams()));
  EXPECT_EQ(status.code(), scada::StatusCode::Good)
      << "second logon was refused by the session the dropped connection left "
         "behind";

  asio_env_.Wait(session.Disconnect());
}

// Regression: the same, for a client that goes away *during* authentication.
//
// CreateSession creates the session in the manager before the connection binds
// it, so a connection that closes while authentication is in flight closed with
// no session to release, and the freshly created one stayed in the session map
// with nothing pointing at it. For a single-session account that was a
// permanent lockout — reproduced against a real `scada-config` tier, where
// every later logon came back Bad_UserIsAlreadyLoggedOn indefinitely and only
// `delete_existing` could clear it.
//
// Two independent things now prevent it, and this test covers the pair: the
// connection releases a session it created but never bound, and the logon gate
// counts only sessions a connection is still serving.
TEST_F(SessionProxyTest, LogonAbandonedDuringAuthDoesNotStrandTheSession) {
  auth_gate_.emplace(asio_env_.any_executor_factory());

  // A raw transport rather than a SessionProxy: the client has to disappear
  // mid-request, and closing the socket under our own control is both closer
  // to the real failure (a killed process) and free of the lifetime hazards of
  // destroying a proxy with its connect coroutine in flight.
  SendCreateSessionAndDropConnection();

  // Pump until the server is inside authentication, i.e. the request arrived
  // and the connection is already gone.
  for (int i = 0; i < 100 && authentications_started_ == 0; ++i) {
    asio_env_.PumpFor(std::chrono::milliseconds{10});
  }
  ASSERT_EQ(authentications_started_, 1)
      << "authentication never started; the gate did not open the window "
         "this test needs";

  // Now let authentication finish, into a connection that is already gone.
  auth_gate_->Complete();
  auth_gate_.reset();
  asio_env_.PumpFor(std::chrono::milliseconds{200});

  SessionProxy session{{.executor_ = asio_env_.any_executor_factory(),
                        .transport_factory_ = asio_env_.transport_factory}};
  auto status = asio_env_.Wait(session.ConnectStatus(GetConnectParams()));
  EXPECT_EQ(status.code(), scada::StatusCode::Good)
      << "the account is locked out by a session that was created for a "
         "connection which had already closed, and that nothing can reach";

  asio_env_.Wait(session.Disconnect());
}

// A session a connection IS still serving must keep refusing a second logon:
// the reclaim above must not have turned the single-session policy off.
TEST_F(SessionProxyTest, LiveSessionStillRefusesASecondLogon) {
  SessionProxy first{{.executor_ = asio_env_.any_executor_factory(),
                      .transport_factory_ = asio_env_.transport_factory}};
  asio_env_.Wait(first.Connect(GetConnectParams()));

  SessionProxy second{{.executor_ = asio_env_.any_executor_factory(),
                       .transport_factory_ = asio_env_.transport_factory}};
  auto status = asio_env_.Wait(second.ConnectStatus(GetConnectParams()));
  EXPECT_EQ(status.code(), scada::StatusCode::Bad_UserIsAlreadyLoggedOn);

  asio_env_.Wait(first.Disconnect());
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
