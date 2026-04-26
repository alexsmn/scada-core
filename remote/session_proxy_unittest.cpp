#include "remote/session_proxy.h"

#include "base/awaitable_promise.h"
#include "base/test/asio_test_environment.h"
#include "base/test/awaitable_test.h"
#include "base/test/network_test_environment.h"
#include "base/test/test_executor.h"
#include "remote/remote_session_manager.h"
#include "remote/session_stub.h"
#include "scada/attribute_service.h"
#include "scada/authentication_adapters.h"
#include "scada/history_service.h"
#include "scada/method_service.h"
#include "scada/node_management_service.h"
#include "scada/status_promise.h"
#include "scada/view_service.h"

#include <gmock/gmock.h>
#include <cassert>
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

class DeferredAttributeService final : public scada::AttributeService {
 public:
  void Read(const scada::ServiceContext&,
            const std::shared_ptr<const std::vector<scada::ReadValueId>>&,
            const scada::ReadCallback& callback) override {
    assert(!read_callback_);
    read_callback_ = callback;
  }

  void Write(const scada::ServiceContext&,
             const std::shared_ptr<const std::vector<scada::WriteValue>>&,
             const scada::WriteCallback& callback) override {
    assert(!write_callback_);
    write_callback_ = callback;
  }

  bool has_pending_read() const { return static_cast<bool>(read_callback_); }
  bool has_pending_write() const { return static_cast<bool>(write_callback_); }

  void CompleteRead(scada::Status status,
                    std::vector<scada::DataValue> results = {}) {
    assert(read_callback_);
    auto callback = std::move(*read_callback_);
    read_callback_.reset();
    callback(std::move(status), std::move(results));
  }

  void CompleteWrite(scada::Status status,
                     std::vector<scada::StatusCode> results = {}) {
    assert(write_callback_);
    auto callback = std::move(*write_callback_);
    write_callback_.reset();
    callback(std::move(status), std::move(results));
  }

 private:
  std::optional<scada::ReadCallback> read_callback_;
  std::optional<scada::WriteCallback> write_callback_;
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

class DeferredMethodService final : public scada::MethodService {
 public:
  void Call(const scada::NodeId&,
            const scada::NodeId&,
            const std::vector<scada::Variant>&,
            const scada::NodeId&,
            const scada::StatusCallback& callback) override {
    assert(!call_callback_);
    call_callback_ = callback;
  }

  bool has_pending_call() const { return static_cast<bool>(call_callback_); }

  void CompleteCall(scada::Status status) {
    assert(call_callback_);
    auto callback = std::move(*call_callback_);
    call_callback_.reset();
    callback(std::move(status));
  }

 private:
  std::optional<scada::StatusCallback> call_callback_;
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

class DeferredHistoryService final : public scada::HistoryService {
 public:
  void HistoryReadRaw(const scada::HistoryReadRawDetails&,
                      const scada::HistoryReadRawCallback& callback) override {
    assert(!history_read_raw_callback_);
    history_read_raw_callback_ = callback;
  }

  void HistoryReadEvents(const scada::NodeId&,
                         base::Time,
                         base::Time,
                         const scada::EventFilter&,
                         const scada::HistoryReadEventsCallback& callback) override {
    callback({});
  }

  bool has_pending_history_read_raw() const {
    return static_cast<bool>(history_read_raw_callback_);
  }

  void CompleteHistoryReadRaw(scada::HistoryReadRawResult result = {}) {
    assert(history_read_raw_callback_);
    auto callback = std::move(*history_read_raw_callback_);
    history_read_raw_callback_.reset();
    callback(std::move(result));
  }

 private:
  std::optional<scada::HistoryReadRawCallback> history_read_raw_callback_;
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

class SessionProxyHarness {
 public:
  SessionProxyHarness(scada::AttributeService* attribute_service,
                      scada::HistoryService* history_service,
                      scada::MethodService* method_service = nullptr)
      : attribute_service_{attribute_service},
        history_service_{history_service},
        method_service_{method_service ? method_service : &default_method_service_},
        session_manager_executor_{std::make_shared<TestExecutor>()} {
    CreateSessionManager();
  }

  ~SessionProxyHarness() {
    session_manager_.reset();
    Drain();
  }

  template <class T>
  T Wait(promise<T> promise) {
    return WaitPromise(std::move(promise), [this] { Poll(); });
  }

  void Wait(promise<> promise) {
    WaitPromise(std::move(promise), [this] { Poll(); });
  }

  void Poll() {
    session_manager_executor_->Poll();
    asio_env_.Poll();
  }

  void Drain() {
    for (int i = 0; i < 1000; ++i) {
      const auto session_manager_task_count =
          session_manager_executor_->GetTaskCount();
      Poll();
      if (session_manager_task_count == 0 &&
          session_manager_executor_->GetTaskCount() == 0) {
        break;
      }
    }
  }

  void ResetSessionManager() {
    session_manager_.reset();
    Drain();
  }

  void CreateSessionManager() {
    session_manager_ = std::make_unique<RemoteSessionManager>(
        RemoteSessionManagerContext{
            .executor_ = MakeTestAnyExecutor(session_manager_executor_),
            .services_ =
                {.attribute_service = attribute_service_,
                 .method_service = method_service_,
                 .history_service = history_service_,
                 .view_service = &view_service_,
                 .node_management_service = &node_management_service_},
            .authenticator_ = scada::MakeCoroutineAuthenticator(
                [](scada::LocalizedText, scada::LocalizedText)
                    -> Awaitable<scada::StatusOr<scada::AuthenticationResult>> {
                  co_return scada::AuthenticationResult{.user_id = {1, 1}};
                }),
            .transport_factory_ = asio_env_.transport_factory,
            .endpoints_ = {transport::TransportString{
                network_env_.server_transport_string}}});

    Wait(session_manager_->Init());
  }

  scada::SessionConnectParams GetConnectParams() const {
    return {.connection_string = network_env_.client_transport_string,
            .user_name = scada::LocalizedText{u"username"},
            .password = scada::LocalizedText{u"password"}};
  }

  AsioTestEnvironment asio_env_;
  NetworkTestEnvironment network_env_;
  NoopMethodService default_method_service_;
  NoopViewService view_service_;
  NoopNodeManagementService node_management_service_;
  scada::AttributeService* attribute_service_;
  scada::HistoryService* history_service_;
  scada::MethodService* method_service_;
  std::shared_ptr<TestExecutor> session_manager_executor_;
  std::unique_ptr<RemoteSessionManager> session_manager_;
};

}  // namespace

class SessionProxyTest : public Test {
 public:
  void SetUp() override {
    session_manager_executor_ = std::make_shared<TestExecutor>();
    CreateSessionManager();
  }

  void TearDown() override {
    session_manager_.reset();
    Drain();
  }

 protected:
  template <class T>
  T Wait(promise<T> promise) {
    return WaitPromise(std::move(promise), [this] { Poll(); });
  }

  void Wait(promise<> promise) {
    WaitPromise(std::move(promise), [this] { Poll(); });
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

  void CreateSessionManager() {
    session_manager_ = std::make_unique<RemoteSessionManager>(
        RemoteSessionManagerContext{
            .executor_ = MakeTestAnyExecutor(session_manager_executor_),
            .services_ =
                {.attribute_service = attribute_service_,
                 .method_service = &method_service_,
                 .history_service = history_service_,
                 .view_service = &view_service_,
                 .node_management_service = &node_management_service_},
            .authenticator_ = scada::MakeCoroutineAuthenticator(
                [this](scada::LocalizedText, scada::LocalizedText)
                    -> Awaitable<scada::StatusOr<scada::AuthenticationResult>> {
                  auth_requested_ = true;
                  if (delay_authentication_) {
                    try {
                      co_return co_await AwaitPromise(
                          MakeTestAnyExecutor(session_manager_executor_),
                          auth_promise_);
                    } catch (...) {
                      co_return scada::GetExceptionStatus(
                          std::current_exception());
                    }
                  }

                  if (auth_failure_status_) {
                    co_return *auth_failure_status_;
                  }

                  co_return scada::AuthenticationResult{.user_id = kUserId};
                }),
            .transport_factory_ = asio_env_.transport_factory,
            .endpoints_ = {transport::TransportString{
                network_env_.server_transport_string}}});

    Wait(session_manager_->Init());
  }

  void ResetSessionManager() {
    session_manager_.reset();
    Drain();
  }

  scada::SessionConnectParams GetConnectParams() const {
    return {.connection_string = network_env_.client_transport_string,
            .user_name = kUserName,
            .password = kPassword};
  }

  AsioTestEnvironment asio_env_;
  NetworkTestEnvironment network_env_;
  NoopAttributeService default_attribute_service_;
  NoopMethodService method_service_;
  NoopViewService view_service_;
  NoopNodeManagementService node_management_service_;
  NoopHistoryService default_history_service_;
  scada::AttributeService* attribute_service_ = &default_attribute_service_;
  scada::HistoryService* history_service_ = &default_history_service_;

  std::optional<scada::StatusCode> auth_failure_status_;
  bool auth_requested_ = false;
  bool delay_authentication_ = false;
  promise<scada::AuthenticationResult> auth_promise_;
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
  SessionProxy session{{.executor_ = asio_env_.any_executor_factory(),
                        .transport_factory_ = asio_env_.transport_factory}};

  Wait(session.Connect(GetConnectParams()));

  EXPECT_TRUE(session.IsConnected(nullptr));

  Wait(session.Disconnect());
}

// Verifies that Connect() with wrong credentials rejects the promise.
TEST_F(SessionProxyTest, Connect_BadPassword) {
  auth_failure_status_ = scada::StatusCode::Bad_WrongLoginCredentials;

  SessionProxy session{{.executor_ = asio_env_.any_executor_factory(),
                        .transport_factory_ = asio_env_.transport_factory}};

  EXPECT_THROW(Wait(session.Connect(GetConnectParams())),
               scada::status_exception);
}

// Verifies that Connect() followed by Disconnect() completes cleanly.
TEST_F(SessionProxyTest, Connect_Disconnect) {
  SessionProxy session{{.executor_ = asio_env_.any_executor_factory(),
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

  SessionProxy session{{.executor_ = asio_env_.any_executor_factory(),
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

  SessionProxy session{{.executor_ = asio_env_.any_executor_factory(),
                        .transport_factory_ = asio_env_.transport_factory}};

  Wait(session.Connect(GetConnectParams()));
  EXPECT_THAT(observer.opened_user_ids, ElementsAre(kUserId));
  EXPECT_THAT(observer.closed_user_ids, IsEmpty());

  Wait(session.Disconnect());
  EXPECT_THAT(observer.closed_user_ids, ElementsAre(kUserId));

  session_manager_->RemoveObserver(observer);
}

TEST_F(SessionProxyTest, ManagerObserverNotNotifiedOnFailedAuthentication) {
  auth_failure_status_ = scada::StatusCode::Bad_WrongLoginCredentials;

  SessionManagerObserver observer;
  session_manager_->AddObserver(observer);

  SessionProxy session{{.executor_ = asio_env_.any_executor_factory(),
                        .transport_factory_ = asio_env_.transport_factory}};

  EXPECT_THROW(Wait(session.Connect(GetConnectParams())),
               scada::status_exception);

  EXPECT_THAT(observer.opened_user_ids, IsEmpty());
  EXPECT_THAT(observer.closed_user_ids, IsEmpty());

  session_manager_->RemoveObserver(observer);
}

TEST_F(SessionProxyTest,
       ManagerObserverNotNotifiedBeforeDelayedAuthenticationCompletes) {
  delay_authentication_ = true;
  auth_requested_ = false;

  SessionManagerObserver observer;
  session_manager_->AddObserver(observer);

  SessionProxy session{{.executor_ = asio_env_.any_executor_factory(),
                        .transport_factory_ = asio_env_.transport_factory}};

  auto connect_promise = session.Connect(GetConnectParams());

  for (int i = 0; i < 1000 && !auth_requested_; ++i) {
    Poll();
  }
  ASSERT_TRUE(auth_requested_);
  EXPECT_THAT(observer.opened_user_ids, IsEmpty());
  EXPECT_THAT(observer.closed_user_ids, IsEmpty());
  EXPECT_FALSE(session.IsConnected(nullptr));

  auth_promise_.resolve(scada::AuthenticationResult{.user_id = kUserId});

  Wait(std::move(connect_promise));
  EXPECT_THAT(observer.opened_user_ids, ElementsAre(kUserId));
  EXPECT_THAT(observer.closed_user_ids, IsEmpty());

  Wait(session.Disconnect());
  session_manager_->RemoveObserver(observer);
}

TEST_F(SessionProxyTest,
       ShutdownDuringPendingCreateSessionRejectsConnectPromise) {
  delay_authentication_ = true;
  auth_requested_ = false;

  SessionProxy session{{.executor_ = asio_env_.any_executor_factory(),
                        .transport_factory_ = asio_env_.transport_factory}};

  auto connect_promise = session.Connect(GetConnectParams());

  for (int i = 0; i < 1000 && !auth_requested_; ++i) {
    Poll();
  }
  ASSERT_TRUE(auth_requested_);

  ResetSessionManager();

  EXPECT_THROW(Wait(std::move(connect_promise)), scada::status_exception);
  EXPECT_FALSE(session.IsConnected(nullptr));
}

TEST_F(SessionProxyTest, Connect_DuplicateUserRejectedWithoutRemoteLogoff) {
  SessionProxy session1{{.executor_ = asio_env_.any_executor_factory(),
                         .transport_factory_ = asio_env_.transport_factory}};
  SessionProxy session2{{.executor_ = asio_env_.any_executor_factory(),
                         .transport_factory_ = asio_env_.transport_factory}};

  Wait(session1.Connect(GetConnectParams()));

  EXPECT_THROW(Wait(session2.Connect(GetConnectParams())),
               scada::status_exception);

  EXPECT_TRUE(session1.IsConnected(nullptr));
  EXPECT_FALSE(session2.IsConnected(nullptr));

  Wait(session1.Disconnect());
}

TEST_F(SessionProxyTest, ManagerObserverNotifiedWhenSessionIsReplaced) {
  auto params = GetConnectParams();
  auto second_params = params;
  second_params.allow_remote_logoff = true;

  SessionManagerObserver observer;
  session_manager_->AddObserver(observer);

  SessionProxy session1{{.executor_ = asio_env_.any_executor_factory(),
                         .transport_factory_ = asio_env_.transport_factory}};
  SessionProxy session2{{.executor_ = asio_env_.any_executor_factory(),
                         .transport_factory_ = asio_env_.transport_factory}};

  Wait(session1.Connect(params));
  Wait(session2.Connect(second_params));
  Poll();

  EXPECT_THAT(observer.opened_user_ids, ElementsAre(kUserId, kUserId));
  EXPECT_THAT(observer.closed_user_ids, ElementsAre(kUserId));

  Wait(session2.Disconnect());

  EXPECT_THAT(observer.closed_user_ids, ElementsAre(kUserId, kUserId));

  session_manager_->RemoveObserver(observer);
}

TEST_F(SessionProxyTest, Connect_DuplicateUserAllowedWithRemoteLogoff) {
  auto params = GetConnectParams();
  auto second_params = params;
  second_params.allow_remote_logoff = true;

  SessionProxy session1{{.executor_ = asio_env_.any_executor_factory(),
                         .transport_factory_ = asio_env_.transport_factory}};
  SessionProxy session2{{.executor_ = asio_env_.any_executor_factory(),
                         .transport_factory_ = asio_env_.transport_factory}};

  Wait(session1.Connect(params));
  Wait(session2.Connect(second_params));

  for (int i = 0; i < 1000 && session1.IsConnected(nullptr); ++i) {
    Poll();
  }

  EXPECT_FALSE(session1.IsConnected(nullptr));
  EXPECT_TRUE(session2.IsConnected(nullptr));

  Wait(session2.Disconnect());
}

TEST_F(SessionProxyTest, CloseUserSessionsDisconnectsMatchingSession) {
  SessionProxy session{{.executor_ = asio_env_.any_executor_factory(),
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
    SessionProxy session{{.executor_ = asio_env_.any_executor_factory(),
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

TEST_F(SessionProxyTest, DisconnectCancelsInFlightReadAndAllowsReconnect) {
  DeferredAttributeService attribute_service;
  NoopHistoryService history_service;
  SessionProxyHarness harness{&attribute_service, &history_service};

  SessionProxy session{{.executor_ = harness.asio_env_.any_executor_factory(),
                        .transport_factory_ = harness.asio_env_.transport_factory}};

  harness.Wait(session.Connect(harness.GetConnectParams()));

  promise<scada::Status> read_status;
  int callback_count = 0;
  session.services().attribute_service->Read(
      {},
      std::make_shared<const std::vector<scada::ReadValueId>>(
          std::vector<scada::ReadValueId>{{.node_id = {1, 2}}}),
      [&](scada::Status status, std::vector<scada::DataValue>) mutable {
        ++callback_count;
        read_status.resolve(status);
      });

  for (int i = 0; i < 1000 && !attribute_service.has_pending_read(); ++i) {
    harness.Poll();
  }
  ASSERT_TRUE(attribute_service.has_pending_read());

  harness.Wait(session.Disconnect());

  EXPECT_EQ(harness.Wait(read_status).code(), scada::StatusCode::Bad_Disconnected);
  EXPECT_EQ(callback_count, 1);

  attribute_service.CompleteRead(scada::StatusCode::Good,
                                 {scada::MakeReadResult(123)});
  harness.Drain();

  EXPECT_EQ(callback_count, 1);

  harness.Wait(session.Connect(harness.GetConnectParams()));
  EXPECT_TRUE(session.IsConnected(nullptr));

  harness.Wait(session.Disconnect());
}

TEST_F(SessionProxyTest, DisconnectCancelsInFlightWriteAndAllowsReconnect) {
  DeferredAttributeService attribute_service;
  NoopHistoryService history_service;
  SessionProxyHarness harness{&attribute_service, &history_service};

  SessionProxy session{{.executor_ = harness.asio_env_.any_executor_factory(),
                        .transport_factory_ = harness.asio_env_.transport_factory}};

  harness.Wait(session.Connect(harness.GetConnectParams()));

  promise<scada::Status> write_status;
  int callback_count = 0;
  session.services().attribute_service->Write(
      {},
      std::make_shared<const std::vector<scada::WriteValue>>(
          std::vector<scada::WriteValue>{{{1, 2},
                                          scada::AttributeId::Value,
                                          scada::Variant{scada::Int32{5}},
                                          {}}}),
      [&](scada::Status status, std::vector<scada::StatusCode>) mutable {
        ++callback_count;
        write_status.resolve(status);
      });

  for (int i = 0; i < 1000 && !attribute_service.has_pending_write(); ++i) {
    harness.Poll();
  }
  ASSERT_TRUE(attribute_service.has_pending_write());

  harness.Wait(session.Disconnect());

  EXPECT_EQ(harness.Wait(write_status).code(),
            scada::StatusCode::Bad_Disconnected);
  EXPECT_EQ(callback_count, 1);

  attribute_service.CompleteWrite(scada::StatusCode::Good,
                                  {scada::StatusCode::Good});
  harness.Drain();

  EXPECT_EQ(callback_count, 1);

  harness.Wait(session.Connect(harness.GetConnectParams()));
  EXPECT_TRUE(session.IsConnected(nullptr));

  harness.Wait(session.Disconnect());
}

TEST_F(SessionProxyTest, DisconnectCancelsInFlightCallAndAllowsReconnect) {
  NoopAttributeService attribute_service;
  NoopHistoryService history_service;
  DeferredMethodService method_service;
  SessionProxyHarness harness{&attribute_service, &history_service,
                              &method_service};

  SessionProxy session{{.executor_ = harness.asio_env_.any_executor_factory(),
                        .transport_factory_ = harness.asio_env_.transport_factory}};

  harness.Wait(session.Connect(harness.GetConnectParams()));

  promise<scada::Status> call_status;
  int callback_count = 0;
  session.services().method_service->Call(
      {1, 2}, {1, 3}, {}, {1, 1},
      [&](scada::Status status) mutable {
        ++callback_count;
        call_status.resolve(status);
      });

  for (int i = 0; i < 1000 && !method_service.has_pending_call(); ++i) {
    harness.Poll();
  }
  ASSERT_TRUE(method_service.has_pending_call());

  harness.Wait(session.Disconnect());

  EXPECT_EQ(harness.Wait(call_status).code(),
            scada::StatusCode::Bad_Disconnected);
  EXPECT_EQ(callback_count, 1);

  method_service.CompleteCall(scada::StatusCode::Good);
  harness.Drain();

  EXPECT_EQ(callback_count, 1);

  harness.Wait(session.Connect(harness.GetConnectParams()));
  EXPECT_TRUE(session.IsConnected(nullptr));

  harness.Wait(session.Disconnect());
}

TEST_F(SessionProxyTest,
       RemoteLogoffCancelsInFlightHistoryReadWithoutLeavingStaleSession) {
  NoopAttributeService attribute_service;
  DeferredHistoryService history_service;
  SessionProxyHarness harness{&attribute_service, &history_service};

  auto params = harness.GetConnectParams();
  auto second_params = params;
  second_params.allow_remote_logoff = true;

  SessionProxy session1{{.executor_ = harness.asio_env_.any_executor_factory(),
                         .transport_factory_ = harness.asio_env_.transport_factory}};
  SessionProxy session2{{.executor_ = harness.asio_env_.any_executor_factory(),
                         .transport_factory_ = harness.asio_env_.transport_factory}};

  harness.Wait(session1.Connect(params));

  promise<scada::Status> history_status;
  int callback_count = 0;
  session1.services().history_service->HistoryReadRaw(
      {.node_id = {1, 3}},
      [&](scada::HistoryReadRawResult result) mutable {
        ++callback_count;
        history_status.resolve(result.status);
      });

  for (int i = 0; i < 1000 && !history_service.has_pending_history_read_raw();
       ++i) {
    harness.Poll();
  }
  ASSERT_TRUE(history_service.has_pending_history_read_raw());

  harness.Wait(session2.Connect(second_params));

  EXPECT_EQ(harness.Wait(history_status).code(),
            scada::StatusCode::Bad_Disconnected);
  EXPECT_EQ(callback_count, 1);
  EXPECT_FALSE(session1.IsConnected(nullptr));
  EXPECT_TRUE(session2.IsConnected(nullptr));

  history_service.CompleteHistoryReadRaw(
      {.status = scada::StatusCode::Good,
       .values = {scada::MakeReadResult(456)}});
  harness.Drain();

  EXPECT_EQ(callback_count, 1);

  harness.Wait(session2.Disconnect());
  harness.Wait(session1.Connect(params));
  EXPECT_TRUE(session1.IsConnected(nullptr));

  harness.Wait(session1.Disconnect());
}

TEST_F(SessionProxyTest, ShutdownReleasesSessionStateAndDisconnectsClient) {
  NoopAttributeService attribute_service;
  NoopHistoryService history_service;
  SessionProxyHarness harness{&attribute_service, &history_service};

  {
    SessionProxy session1{{.executor_ = harness.asio_env_.any_executor_factory(),
                           .transport_factory_ =
                               harness.asio_env_.transport_factory}};

    harness.Wait(session1.Connect(harness.GetConnectParams()));
    EXPECT_TRUE(session1.IsConnected(nullptr));

    harness.ResetSessionManager();

    for (int i = 0; i < 1000 && session1.IsConnected(nullptr); ++i) {
      harness.Poll();
    }

    EXPECT_FALSE(session1.IsConnected(nullptr));
  }
}
