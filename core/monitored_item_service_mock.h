#pragma once

#include "core/monitored_item_mock.h"
#include "core/monitored_item_service.h"

#include <gmock/gmock.h>

namespace scada {

using MockDataChangeHandler =
    testing::MockFunction<void(const DataValue& data_value)>;

using MockEventHandler =
    testing::MockFunction<void(const Status& status, const std::any& event)>;

class MockMonitoredItemService : public MonitoredItemService {
 public:
  MockMonitoredItemService() {
    using namespace testing;

    ON_CALL(*this, CreateMonitoredItem(_, _))
        .WillByDefault(Return(default_monitored_item));
  }

  MOCK_METHOD2(
      CreateMonitoredItem,
      std::shared_ptr<MonitoredItem>(const ReadValueId& value_id,
                                     const MonitoringParameters& params));

  const std::shared_ptr<MockMonitoredItem> default_monitored_item =
      std::make_shared<testing::NiceMock<MockMonitoredItem>>();
};

}  // namespace scada
