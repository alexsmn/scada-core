#include "scada/attribute_service.h"

#include "base/test/awaitable_test.h"
#include "scada/attribute_service_mock.h"
#include "scada/service_context.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace scada {
namespace {

using testing::_;
using testing::Invoke;

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

  auto read_result = WaitAwaitable(executor, adapter.Read(context, read_inputs));
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

}  // namespace
}  // namespace scada
