#pragma once

#include "core/monitored_item.h"

#include <gmock/gmock.h>

namespace scada {

class MockMonitoredItem : public MonitoredItem {
 public:
  const DataChangeHandler& data_change_handler() const {
    return data_change_handler_;
  }

  MOCK_METHOD(void, Subscribe, ());
};

}  // namespace scada