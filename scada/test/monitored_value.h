#pragma once

#include "scada/client.h"

namespace scada {

struct monitored_value {
  const DataValue& data_value() const { return data_value_; }

  void subscribe_value(const scada::node& node) {
    // TODO: Captures `this`.
    monitored_item_ = node.subscribe_value(
        [this](const DataValue& data_value) { data_value_ = data_value; });
  }

 private:
  monitored_item monitored_item_;
  DataValue data_value_;
};

}  // namespace scada
