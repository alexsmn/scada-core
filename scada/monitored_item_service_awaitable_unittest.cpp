#include "scada/monitored_item_service_awaitable.h"

#include "base/awaitable_promise.h"
#include "base/test/awaitable_test.h"
#include "base/test/test_executor.h"
#include "scada/monitoring_parameters.h"
#include "scada/read_value_id.h"
#include "scada/test/test_monitored_item.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <map>

namespace scada {
namespace {

using namespace testing;

class TestMonitoredItemService : public MonitoredItemService {
 public:
  std::shared_ptr<MonitoredItem> CreateMonitoredItem(
      const ReadValueId& value_id,
      const MonitoringParameters& params) override {
    (void)params;
    auto i = items.find(value_id.node_id);
    return i == items.end() ? nullptr : i->second;
  }

  std::map<NodeId, std::shared_ptr<TestMonitoredItem>> items;
};

TEST(MonitoredItemServiceAwaitable, ReadsInitialValuesInInputOrder) {
  auto executor = std::make_shared<TestExecutor>();
  TestMonitoredItemService service;
  auto first_item = std::make_shared<TestMonitoredItem>();
  auto second_item = std::make_shared<TestMonitoredItem>();
  service.items.emplace(NodeId{1, 1}, first_item);
  service.items.emplace(NodeId{2, 1}, second_item);

  const std::vector<ReadValueId> inputs{{NodeId{1, 1}}, {NodeId{2, 1}}};
  auto read_promise = ToPromise(
      MakeTestAnyExecutor(executor),
      ReadInitialValuesAsync(MakeTestAnyExecutor(executor), service, inputs,
                             /*params=*/{}));

  Drain(executor);
  second_item->NotifyDataChange(
      DataValue{Variant{22}, {}, DateTime{}, DateTime{}});
  Drain(executor);
  first_item->NotifyDataChange(
      DataValue{Variant{11}, {}, DateTime{}, DateTime{}});

  EXPECT_THAT(WaitPromise(executor, std::move(read_promise)),
              ElementsAre(Field(&DataValue::value, Variant{11}),
                          Field(&DataValue::value, Variant{22})));
}

TEST(MonitoredItemServiceAwaitable, ReturnsReadErrorForMissingItem) {
  auto executor = std::make_shared<TestExecutor>();
  TestMonitoredItemService service;

  auto read_promise = ToPromise(
      MakeTestAnyExecutor(executor),
      ReadInitialValueAsync(MakeTestAnyExecutor(executor), service,
                            ReadValueId{NodeId{1, 1}}, /*params=*/{}));

  EXPECT_THAT(WaitPromise(executor, std::move(read_promise)),
              Field(&DataValue::status_code, StatusCode::Bad_WrongNodeId));
}

}  // namespace
}  // namespace scada
