#include "scada/coroutine_services.h"

#include "base/test/awaitable_test.h"
#include "scada/attribute_service_mock.h"
#include "scada/session_service_mock.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace scada {
namespace {

using testing::_;
using testing::Invoke;

class TestCoroutineSessionService final : public CoroutineSessionService {
 public:
  Awaitable<void> Connect(SessionConnectParams params) override {
    last_connect_params = std::move(params);
    connected = true;
    co_return;
  }

  Awaitable<void> Reconnect() override {
    reconnect_called = true;
    co_return;
  }

  Awaitable<void> Disconnect() override {
    disconnect_called = true;
    connected = false;
    co_return;
  }

  bool IsConnected(base::TimeDelta* ping_delay = nullptr) const override {
    return connected;
  }

  NodeId GetUserId() const override { return user_id; }

  bool HasPrivilege(Privilege privilege) const override {
    return privilege == allowed_privilege;
  }

  std::string GetHostName() const override { return host_name; }
  bool IsScada() const override { return is_scada; }

  boost::signals2::scoped_connection SubscribeSessionStateChanged(
      const SessionStateChangedCallback& callback) override {
    state_changed_callback = callback;
    return {};
  }

  SessionDebugger* GetSessionDebugger() override { return debugger; }

  SessionConnectParams last_connect_params;
  SessionStateChangedCallback state_changed_callback;
  SessionDebugger* debugger = nullptr;
  NodeId user_id{9};
  Privilege allowed_privilege = Privilege::Control;
  std::string host_name = "test-host";
  bool connected = false;
  bool is_scada = true;
  bool reconnect_called = false;
  bool disconnect_called = false;
};

TEST(CallbackToCoroutineAttributeServiceAdapter, ReadAndWriteForwardResults) {
  auto executor = std::make_shared<TestExecutor>();
  testing::StrictMock<MockAttributeService> service;
  CallbackToCoroutineAttributeServiceAdapter adapter{executor, service};

  const auto context = ServiceContext{}.with_request_id(17);
  auto read_inputs =
      std::make_shared<std::vector<ReadValueId>>(1, ReadValueId{.node_id = 1});

  EXPECT_CALL(service, Read(_, _, _))
      .WillOnce(Invoke([&](const ServiceContext& actual_context,
                           const std::shared_ptr<const std::vector<ReadValueId>>&
                               actual_inputs,
                           const ReadCallback& callback) {
        EXPECT_EQ(actual_context.request_id(), context.request_id());
        ASSERT_EQ(actual_inputs->size(), 1u);
        EXPECT_EQ((*actual_inputs)[0], (*read_inputs)[0]);
        callback(StatusCode::Good, {DataValue{Variant{42}, {}, {}, {}}});
      }));

  auto read_result =
      WaitAwaitable(executor, adapter.Read(context, read_inputs));
  ASSERT_TRUE(read_result.ok());
  ASSERT_EQ(read_result->size(), 1u);
  EXPECT_EQ((*read_result)[0], DataValue(Variant{42}, {}, {}, {}));

  auto write_inputs = std::make_shared<std::vector<WriteValue>>(
      1, WriteValue{.node_id = 2, .value = Variant{11}});
  EXPECT_CALL(service, Write(_, _, _))
      .WillOnce(Invoke([&](const ServiceContext& actual_context,
                           const std::shared_ptr<const std::vector<WriteValue>>&
                               actual_inputs,
                           const WriteCallback& callback) {
        EXPECT_EQ(actual_context.request_id(), context.request_id());
        ASSERT_EQ(actual_inputs->size(), 1u);
        EXPECT_EQ((*actual_inputs)[0], (*write_inputs)[0]);
        callback(StatusCode::Good, {StatusCode::Good});
      }));

  auto write_result =
      WaitAwaitable(executor, adapter.Write(context, write_inputs));
  ASSERT_TRUE(write_result.ok());
  EXPECT_THAT(*write_result, testing::ElementsAre(StatusCode::Good));
}

TEST(SessionToCoroutineSessionServiceAdapter, ForwardsCoroutineSessionService) {
  auto executor = std::make_shared<TestExecutor>();
  testing::StrictMock<MockSessionService> service;
  SessionToCoroutineSessionServiceAdapter adapter{service};

  SessionConnectParams params{.host = "opc.tcp://localhost:4840"};
  EXPECT_CALL(service, Connect(_))
      .WillOnce(Invoke([&](SessionConnectParams actual_params)
                           -> Awaitable<void> {
        EXPECT_EQ(actual_params.host, params.host);
        co_return;
      }));

  EXPECT_NO_THROW(WaitAwaitable(executor, adapter.Connect(params)));

  EXPECT_CALL(service, IsConnected(_)).WillOnce(Invoke([](base::TimeDelta*) {
    return true;
  }));
  EXPECT_TRUE(adapter.IsConnected());
}

TEST(CoroutineToCallbackAttributeServiceAdapter,
     ConvertsCoroutineExceptionsToBadStatus) {
  class ThrowingAttributeService final : public CoroutineAttributeService {
   public:
    Awaitable<StatusOr<std::vector<DataValue>>> Read(
        ServiceContext,
        std::shared_ptr<const std::vector<ReadValueId>>) override {
      throw status_exception{StatusCode::Bad};
    }

    Awaitable<StatusOr<std::vector<StatusCode>>> Write(
        ServiceContext,
        std::shared_ptr<const std::vector<WriteValue>>) override {
      co_return std::vector{StatusCode::Good};
    }
  };

  auto executor = std::make_shared<TestExecutor>();
  ThrowingAttributeService service;
  CoroutineToCallbackAttributeServiceAdapter adapter{executor, service};

  StatusCode status_code = StatusCode::Good;
  bool called = false;
  auto inputs =
      std::make_shared<std::vector<ReadValueId>>(1, ReadValueId{.node_id = 1});
  adapter.Read(ServiceContext{}, inputs,
               [&](Status status, std::vector<DataValue> results) {
                 EXPECT_TRUE(results.empty());
                 status_code = status.code();
                 called = true;
               });
  Drain(executor);

  EXPECT_TRUE(called);
  EXPECT_EQ(status_code, StatusCode::Bad);
}

TEST(CoroutineToSessionServiceAdapter, ConnectsAndPassesThroughState) {
  auto executor = std::make_shared<TestExecutor>();
  TestCoroutineSessionService service;
  CoroutineToSessionServiceAdapter adapter{service};

  SessionConnectParams params{.host = "opc.tcp://127.0.0.1:4840"};
  WaitAwaitable(executor, adapter.Connect(params));
  EXPECT_EQ(service.last_connect_params.host, params.host);
  EXPECT_TRUE(service.connected);

  EXPECT_EQ(adapter.GetUserId(), service.user_id);
  EXPECT_TRUE(adapter.HasPrivilege(service.allowed_privilege));
  EXPECT_EQ(adapter.GetHostName(), service.host_name);
  EXPECT_TRUE(adapter.IsScada());

  WaitAwaitable(executor, adapter.Disconnect());
  EXPECT_TRUE(service.disconnect_called);
  EXPECT_FALSE(service.connected);
}

}  // namespace
}  // namespace scada
