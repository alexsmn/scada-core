#pragma once

#include "scada/monitored_item.h"

#include <gmock/gmock.h>

namespace scada {

class MockMonitoredItem : public MonitoredItem {
 public:
  MockMonitoredItem() {
    ON_CALL(
        *this,
        Subscribe(testing::VariantWith<scada::DataChangeHandler>(testing::_)))
        .WillByDefault(testing::Invoke([this](MonitoredItemHandler handler) {
          data_change_handler =
              std::move(std::get<scada::DataChangeHandler>(handler));
        }));
  }

  MOCK_METHOD(void, Subscribe, (MonitoredItemHandler handler), (override));

  scada::DataChangeHandler data_change_handler;
};

}  // namespace scada
