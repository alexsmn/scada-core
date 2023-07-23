#pragma once

#include "scada/client.h"

namespace scada {

struct awaiter {
  explicit awaiter(const scada::node& node)
      : monitored_item_{node.subscribe_value(
            std::bind_front(&state::handle_data_change, state_))} {}

  template <class T>
  promise<scada::DataValue> when(T&& matcher) {
    if (state_->data_value_.has_value() &&
        matcher(state_->data_value_.value())) {
      return make_resolved_promise(state_->data_value_.value());
    }

    return state_->matchers_
        .emplace_back(std::forward<T>(matcher), promise<scada::DataValue>{})
        .second;
  }

  promise<scada::DataValue> when_any() {
    return when([](const scada::DataValue& data_value) { return true; });
  }

 private:
  struct state {
    void handle_data_change(const scada::DataValue& data_value) {
      data_value_ = data_value;

      auto matching = std::ranges::partition(
          matchers_, [&data_value](const auto& m) { return !m(data_value); },
          [](const auto& p) { return p.first; });

      for (auto& [m, p] : matching) {
        p.resolve(data_value);
      }

      matchers_.erase(matching.begin(), matching.end());
    }

    std::optional<scada::DataValue> data_value_;

    using matcher = std::function<bool(const scada::DataValue& data_value)>;
    std::vector<std::pair<matcher, promise<scada::DataValue>>> matchers_;
  };

  const std::shared_ptr<state> state_ = std::make_shared<state>();
  const scada::monitored_item monitored_item_;
};

}  // namespace scada
