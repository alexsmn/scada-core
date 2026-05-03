#include "scada/attribute_service.h"

#include "base/test/awaitable_test.h"
#include "scada/attribute_service_mock.h"
#include "scada/service_context.h"
#include "scada/status_exception.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace scada {
namespace {

using testing::Invoke;

TEST(AttributeServiceHelpers, ReadAndWriteForwardResults) {
  TestExecutor executor;
  testing::StrictMock<MockAttributeService> service;

  const auto context = ServiceContext{}.with_request_id(17);
  auto read_inputs =
      std::make_shared<std::vector<ReadValueId>>(1, ReadValueId{.node_id = 1});

  EXPECT_CALL(service, Read(testing::_, testing::_))
      .WillOnce(Invoke([&](ServiceContext actual_context,
                           std::shared_ptr<const std::vector<ReadValueId>>
                               actual_inputs) -> Awaitable<StatusOr<std::vector<DataValue>>> {
        EXPECT_EQ(actual_context.request_id(), context.request_id());
        EXPECT_EQ(actual_inputs->size(), 1u);
        if (actual_inputs->size() != 1u)
          co_return StatusCode::Bad;
        EXPECT_EQ((*actual_inputs)[0], (*read_inputs)[0]);
        co_return std::vector{DataValue{Variant{42}, {}, {}, {}}};
      }));

  auto read_result = WaitAwaitable(executor, service.Read(context, read_inputs));
  ASSERT_TRUE(read_result.ok());
  ASSERT_EQ(read_result->size(), 1u);
  EXPECT_EQ((*read_result)[0], DataValue(Variant{42}, {}, {}, {}));

  auto write_inputs = std::make_shared<std::vector<WriteValue>>(
      1, WriteValue{.node_id = 2, .value = Variant{11}});
  EXPECT_CALL(service, Write(testing::_, testing::_))
      .WillOnce(Invoke([&](ServiceContext actual_context,
                           std::shared_ptr<const std::vector<WriteValue>>
                               actual_inputs) -> Awaitable<StatusOr<std::vector<StatusCode>>> {
        EXPECT_EQ(actual_context.request_id(), context.request_id());
        EXPECT_EQ(actual_inputs->size(), 1u);
        if (actual_inputs->size() != 1u)
          co_return StatusCode::Bad;
        EXPECT_EQ((*actual_inputs)[0], (*write_inputs)[0]);
        co_return std::vector{StatusCode::Good};
      }));

  auto write_result =
      WaitAwaitable(executor, service.Write(context, write_inputs));
  ASSERT_TRUE(write_result.ok());
  EXPECT_THAT(*write_result, testing::ElementsAre(StatusCode::Good));
}

TEST(AttributeServiceHelpers, PropagatesCoroutineExceptions) {
  class ThrowingAttributeService final : public AttributeService {
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

  TestExecutor executor;
  ThrowingAttributeService service;
  auto inputs =
      std::make_shared<std::vector<ReadValueId>>(1, ReadValueId{.node_id = 1});

  EXPECT_THROW(
      {
        auto result = WaitAwaitable(executor,
                                    service.Read(ServiceContext{}, inputs));
        (void)result;
      },
      status_exception);
}

}  // namespace
}  // namespace scada
