#include "remote/session_proxy.h"

#include "base/test/asio_test_environment.h"
#include "base/test/network_test_environment.h"
#include "base/test/test_executor.h"
#include "remote/remote_session_manager.h"
#include "remote/session_stub.h"
#include "scada/attribute_service.h"
#include "scada/history_service.h"
#include "scada/method_service.h"
#include "scada/node_management_service.h"
#include "scada/status_promise.h"
#include "scada/view_service.h"

#include <gmock/gmock.h>
#include <optional>

using namespace testing;

namespace {

class NoopAttributeService final : public scada::AttributeService {
 public:
  void Read(const scada::ServiceContext&,
            const std::shared_ptr<const std::vector<scada::ReadValueId>>&,
            const scada::ReadCallback& callback) override {
    callback(scada::StatusCode::Good, {});
  }

  void Write(const scada::ServiceContext&,
             const std::shared_ptr<const std::vector<scada::WriteValue>>&,
             const scada::WriteCallback& callback) override {
    callback(scada::StatusCode::Good, {});
  }
};

class NoopMethodService final : public scada::MethodService {
 public:
  void Call(const scada::NodeId&,
            const scada::NodeId&,
            const std::vector<scada::Variant>&,
            const scada::NodeId&,
            const scada::StatusCallback& callback) override {
    callback(scada::StatusCode::Good);
  }
};

class NoopViewService final : public scada::ViewService {
 public:
  void Browse(const scada::ServiceContext&,
              const std::vector<scada::BrowseDescription>&,
              const scada::BrowseCallback& callback) override {
    callback(scada::StatusCode::Good, {});
  }

  void TranslateBrowsePaths(
      const std::vector<scada::BrowsePath>&,
      const scada::TranslateBrowsePathsCallback& callback) override {
    callback(scada::StatusCode::Good, {});
  }
};

class NoopNodeManagementService final : public scada::NodeManagementService {
 public:
  void AddNodes(const std::vector<scada::AddNodesItem>&,
                const scada::AddNodesCallback& callback) override {
    callback(scada::StatusCode::Good, {});
  }

  void DeleteNodes(const std::vector<scada::DeleteNodesItem>&,
                   const scada::DeleteNodesCallback& callback) override {
    callback(scada::StatusCode::Good, {});
  }

  void AddReferences(const std::vector<scada::AddReferencesItem>&,
                     const scada::AddReferencesCallback& callback) override {
    callback(scada::StatusCode::Good, {});
  }

  void DeleteReferences(
      const std::vector<scada::DeleteReferencesItem>&,
      const scada::DeleteReferencesCallback& callback) override {
    callback(scada::StatusCode::Good, {});
  }
};

class NoopHistoryService final : public scada::HistoryService {
 public:
  void HistoryReadRaw(const scada::HistoryReadRawDetails&,
                      const scada::HistoryReadRawCallback& callback) override {
    callback({});
  }

  void HistoryReadEvents(
      const scada::NodeId&,
      base::Time,
      base::Time,
      const scada::EventFilter&,
      const scada::HistoryReadEventsCallback& callback) override {
    callback({});
  }
};

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

}  // namespace

class SessionProxyTest : public Test {
 public:
  void SetUp() override {
    session_manager_executor_ = std::make_shared<TestExecutor>();
    session_manager_ = std::make_unique<RemoteSessionManager>(
        RemoteSessionManagerContext{
            .executor_ = session_manager_executor_,
            .services_ =
                {.attribute_service = &attribute_service_,
                 .method_service = &method_service_,
                 .history_service = &history_service_,
                 .view_service = &view_service_,
                 .node_management_service = &node_management_service_},
            .authenticator_ =
                [this](const scada::LocalizedText&,
                       const scada::LocalizedText&) {
                  if (auth_failure_status_) {
                    return scada::MakeRejectedStatusPromise<
                        scada::AuthenticationResult>(*auth_failure_status_);
                  }

                  return make_resolved_promise(
                      scada::AuthenticationResult{.user_id = kUserId});
                },
            .transport_factory_ = asio_env_.transport_factory,
            .endpoints_ = {transport::TransportString{
                network_env_.server_transport_string}}});

    Wait(session_manager_->Init());
  }

  void TearDown() override {
    session_manager_.reset();
    Drain();
  }

 protected:
  template <class T>
  T Wait(promise<T> promise) {
    using namespace std::chrono_literals;
    while (promise.wait_for(1ms) == promise_wait_status::timeout) {
      Poll();
    }
    return promise.get();
  }

  void Wait(promise<> promise) {
    using namespace std::chrono_literals;
    while (promise.wait_for(1ms) == promise_wait_status::timeout) {
      Poll();
    }
    promise.get();
  }

  void Poll() {
    if (session_manager_executor_) {
      session_manager_executor_->Poll();
    }
    asio_env_.Poll();
  }

  void Drain() {
    for (int i = 0; i < 1000; ++i) {
      const auto session_manager_task_count =
          session_manager_executor_ ? session_manager_executor_->GetTaskCount()
                                    : 0;
      Poll();
      if (session_manager_task_count == 0 &&
          session_manager_executor_ && session_manager_executor_->GetTaskCount() == 0) {
        break;
      }
    }
  }

  scada::SessionConnectParams GetConnectParams() const {
    return {.connection_string = network_env_.client_transport_string,
            .user_name = kUserName,
            .password = kPassword};
  }

  AsioTestEnvironment asio_env_;
  NetworkTestEnvironment network_env_;
  NoopAttributeService attribute_service_;
  NoopMethodService method_service_;
  NoopViewService view_service_;
  NoopNodeManagementService node_management_service_;
  NoopHistoryService history_service_;

  std::optional<scada::StatusCode> auth_failure_status_;
  std::shared_ptr<TestExecutor> session_manager_executor_;
  std::unique_ptr<RemoteSessionManager> session_manager_;

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

  Wait(session.Connect(GetConnectParams()));

  EXPECT_TRUE(session.IsConnected(nullptr));

  Wait(session.Disconnect());
}

// Verifies that Connect() with wrong credentials rejects the promise.
TEST_F(SessionProxyTest, Connect_BadPassword) {
  auth_failure_status_ = scada::StatusCode::Bad_WrongLoginCredentials;

  SessionProxy session{{.executor_ = asio_env_.executor,
                        .transport_factory_ = asio_env_.transport_factory}};

  EXPECT_THROW(Wait(session.Connect(GetConnectParams())),
               scada::status_exception);
}

// Verifies that Connect() followed by Disconnect() completes cleanly.
TEST_F(SessionProxyTest, Connect_Disconnect) {
  SessionProxy session{{.executor_ = asio_env_.executor,
                        .transport_factory_ = asio_env_.transport_factory}};

  Wait(session.Connect(GetConnectParams()));
  EXPECT_TRUE(session.IsConnected(nullptr));

  Wait(session.Disconnect());
  EXPECT_FALSE(session.IsConnected(nullptr));
}

// Verifies that Reconnect() resolves the connect_promise_ after
// disconnect+reconnect cycle.
TEST_F(SessionProxyTest, Reconnect) {
  auto params = GetConnectParams();
  params.allow_remote_logoff = true;

  SessionProxy session{{.executor_ = asio_env_.executor,
                        .transport_factory_ = asio_env_.transport_factory}};

  Wait(session.Connect(params));
  EXPECT_TRUE(session.IsConnected(nullptr));

  Wait(session.Reconnect());
  EXPECT_TRUE(session.IsConnected(nullptr));

  Wait(session.Disconnect());
}

TEST_F(SessionProxyTest, ManagerObserverNotifiedOnOpenAndClose) {
  SessionManagerObserver observer;
  session_manager_->AddObserver(observer);

  SessionProxy session{{.executor_ = asio_env_.executor,
                        .transport_factory_ = asio_env_.transport_factory}};

  Wait(session.Connect(GetConnectParams()));
  EXPECT_THAT(observer.opened_user_ids, ElementsAre(kUserId));
  EXPECT_THAT(observer.closed_user_ids, IsEmpty());

  Wait(session.Disconnect());
  EXPECT_THAT(observer.closed_user_ids, ElementsAre(kUserId));

  session_manager_->RemoveObserver(observer);
}

TEST_F(SessionProxyTest, Connect_DuplicateUserRejectedWithoutRemoteLogoff) {
  SessionProxy session1{{.executor_ = asio_env_.executor,
                         .transport_factory_ = asio_env_.transport_factory}};
  SessionProxy session2{{.executor_ = asio_env_.executor,
                         .transport_factory_ = asio_env_.transport_factory}};

  Wait(session1.Connect(GetConnectParams()));

  EXPECT_THROW(Wait(session2.Connect(GetConnectParams())),
               scada::status_exception);

  EXPECT_TRUE(session1.IsConnected(nullptr));
  EXPECT_FALSE(session2.IsConnected(nullptr));

  Wait(session1.Disconnect());
}

TEST_F(SessionProxyTest, Connect_DuplicateUserAllowedWithRemoteLogoff) {
  auto params = GetConnectParams();
  auto second_params = params;
  second_params.allow_remote_logoff = true;

  SessionProxy session1{{.executor_ = asio_env_.executor,
                         .transport_factory_ = asio_env_.transport_factory}};
  SessionProxy session2{{.executor_ = asio_env_.executor,
                         .transport_factory_ = asio_env_.transport_factory}};

  Wait(session1.Connect(params));
  Wait(session2.Connect(second_params));
  Poll();

  EXPECT_FALSE(session1.IsConnected(nullptr));
  EXPECT_TRUE(session2.IsConnected(nullptr));

  Wait(session2.Disconnect());
}

TEST_F(SessionProxyTest, CloseUserSessionsDisconnectsMatchingSession) {
  SessionProxy session{{.executor_ = asio_env_.executor,
                        .transport_factory_ = asio_env_.transport_factory}};

  Wait(session.Connect(GetConnectParams()));
  EXPECT_TRUE(session.IsConnected(nullptr));

  session_manager_->CloseUserSessions(kUserId);

  for (int i = 0; i < 1000 && session.IsConnected(nullptr); ++i) {
    Poll();
  }

  EXPECT_FALSE(session.IsConnected(nullptr));
}

TEST_F(SessionProxyTest, ClientTransportDropClosesServerSession) {
  SessionManagerObserver observer;
  session_manager_->AddObserver(observer);

  {
    SessionProxy session{{.executor_ = asio_env_.executor,
                          .transport_factory_ = asio_env_.transport_factory}};

    Wait(session.Connect(GetConnectParams()));
    EXPECT_THAT(observer.opened_user_ids, ElementsAre(kUserId));
    EXPECT_THAT(observer.closed_user_ids, IsEmpty());
  }

  for (int i = 0; i < 1000 && observer.closed_user_ids.empty(); ++i) {
    Poll();
  }

  EXPECT_THAT(observer.closed_user_ids, ElementsAre(kUserId));

  session_manager_->RemoveObserver(observer);
}
