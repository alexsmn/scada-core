#pragma once

#include "core/monitored_item_mock.h"
#include "core/monitored_item_service.h"

#include <gmock/gmock.h>

namespace scada {

class MockMonitoredItemService : public MonitoredItemService {
 public:
  MockMonitoredItemService() {
    using namespace testing;

    ON_CALL(*this, CreateMonitoredItem(_, _))
        .WillByDefault(Return(monitored_item_));
  }

  MOCK_METHOD2(
      CreateMonitoredItem,
      std::shared_ptr<MonitoredItem>(const ReadValueId& value_id,
                                     const MonitoringParameters& params));

  std::shared_ptr<MockMonitoredItem> monitored_item_ =
      std::make_shared<testing::NiceMock<MockMonitoredItem>>();
};

}  // namespace scada
