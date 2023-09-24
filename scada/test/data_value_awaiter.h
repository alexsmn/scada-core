#pragma once

#include "scada/client_monitored_item.h"

namespace scada {

struct data_value_awaiter {
  explicit data_value_awaiter(const scada::node& node) {
    monitored_item_.subscribe_value(
        node, /*params*/ {},
        std::bind_front(&state::handle_data_change, state_));
    // TODO: Handle subscription failure, when `!monitored_item_.subscribed()`.
  }

  template <class T>
  promise<scada::DataValue> when(T&& matcher) {
    return state_->when(std::forward<T>(matcher));
  }

  promise<scada::DataValue> when_any() {
    return state_->when(
        [](const scada::DataValue& data_value) { return true; });
  }

 private:
  struct state {
    template <class T>
    promise<scada::DataValue> when(T&& matcher) {
      if (current_data_value_.has_value() &&
          matcher(current_data_value_.value())) {
        return make_resolved_promise(current_data_value_.value());
      }

      return matchers_
          .emplace_back(std::forward<T>(matcher), promise<scada::DataValue>{})
          .second;
    }

    void handle_data_change(const scada::DataValue& data_value) {
      current_data_value_ = data_value;

      auto matching = std::ranges::partition(
          matchers_, [&data_value](const auto& m) { return !m(data_value); },
          [](const auto& p) { return p.first; });

      for (auto& [m, p] : matching) {
        p.resolve(data_value);
      }

      matchers_.erase(matching.begin(), matching.end());
    }

    std::optional<scada::DataValue> current_data_value_;

    using matcher = std::function<bool(const scada::DataValue& data_value)>;
    std::vector<std::pair<matcher, promise<scada::DataValue>>> matchers_;
  };

  const std::shared_ptr<state> state_ = std::make_shared<state>();
  scada::monitored_item monitored_item_;
};

}  // namespace scada
