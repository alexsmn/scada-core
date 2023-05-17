#pragma once

#include "core/client.h"

namespace scada {

struct awaiter {
  explicit awaiter(const scada::node& node)
      : monitored_item_{node.subscribe_value(
            std::bind_front(&state::handle_data_change, state_))} {}

  template <class T>
  promise<> when(T&& matcher) {
    if (matcher(state_->data_value_)) {
      return make_resolved_promise();
    }

    return state_->matchers_.emplace_back(std::forward<T>(matcher), promise<>{})
        .second;
  }

 private:
  struct state {
    void handle_data_change(const scada::DataValue& data_value) {
      data_value_ = data_value;

      auto matching = std::ranges::partition(
          matchers_, [&data_value](const auto& m) { return !m(data_value); },
          [](const auto& p) { return p.first; });

      for (auto& [m, p] : matching) {
        p.resolve();
      }

      matchers_.erase(matching.begin(), matching.end());
    }

    scada::DataValue data_value_;

    using matcher = std::function<bool(const scada::DataValue& data_value)>;
    std::vector<std::pair<matcher, promise<>>> matchers_;
  };

  const std::shared_ptr<state> state_ = std::make_shared<state>();
  const scada::monitored_item monitored_item_;
};

}  // namespace scada
