#pragma once

#include "base/callback_awaitable.h"
#include "base/any_executor.h"
#include "scada/client_monitored_item.h"
#include "scada/monitoring_parameters.h"

#include <boost/asio/this_coro.hpp>

namespace scada {

struct data_value_awaiter {
  explicit data_value_awaiter(const scada::node& node) {
    monitored_item_.subscribe_value(
        node, /*params=*/{},
        std::bind_front(&state::handle_data_change, state_));
  }

  template <class T>
  Awaitable<scada::DataValue> when(T matcher) {
    return state_->when(std::move(matcher));
  }

  Awaitable<scada::DataValue> when_any() {
    return state_->when(
        [](const scada::DataValue& data_value) { return true; });
  }

 private:
  struct state {
    template <class T>
    Awaitable<scada::DataValue> when(T matcher) {
      if (current_data_value_.has_value() &&
          matcher(current_data_value_.value())) {
        co_return current_data_value_.value();
      }

      auto executor = co_await boost::asio::this_coro::executor;
      auto [data_value] = co_await CallbackToAwaitable<scada::DataValue>(
          executor,
          [this, matcher = std::move(matcher)](auto callback) mutable {
            matchers_.emplace_back(std::move(matcher), std::move(callback));
          });
      co_return std::move(data_value);
    }

    void handle_data_change(const scada::DataValue& data_value) {
      current_data_value_ = data_value;

      auto matching = std::ranges::partition(
          matchers_, [&data_value](const auto& m) { return !m(data_value); },
          [](const auto& p) { return p.first; });

      for (auto& [m, callback] : matching) {
        callback(data_value);
      }

      matchers_.erase(matching.begin(), matching.end());
    }

    std::optional<scada::DataValue> current_data_value_;

    using matcher = std::function<bool(const scada::DataValue& data_value)>;
    using callback = std::function<void(scada::DataValue)>;
    std::vector<std::pair<matcher, callback>> matchers_;
  };

  const std::shared_ptr<state> state_ = std::make_shared<state>();
  scada::monitored_item monitored_item_;
};

}  // namespace scada
