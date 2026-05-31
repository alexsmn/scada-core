#pragma once

#include "base/callback_awaitable.h"
#include "base/any_executor.h"
#include "scada/client.h"

#include <boost/asio/this_coro.hpp>

namespace scada {

struct event_awaiter {
  explicit event_awaiter(const scada::node& node,
                         const scada::MonitoringParameters& params = {}) {
    monitored_item_.subscribe_system_events(
        node, params, std::bind_front(&state::handle_system_event, state_));
  }

  template <class T>
  Awaitable<scada::Event> when(T matcher) {
    return state_->when(std::move(matcher));
  }

  Awaitable<scada::Event> when_node(const scada::NodeId& node_id) {
    return state_->when([node_id](const scada::Event& event) {
      return event.node_id == node_id;
    });
  }

  Awaitable<scada::Event> when_any() {
    return state_->when([](const scada::Event& event) { return true; });
  }

 private:
  struct state {
    template <class T>
    Awaitable<scada::Event> when(T matcher) {
      auto executor = co_await boost::asio::this_coro::executor;
      auto [status, event] = co_await CallbackToAwaitable<Status, scada::Event>(
          executor,
          [this, matcher = std::move(matcher)](auto callback) mutable {
            matchers_.emplace_back(std::move(matcher), std::move(callback));
          });
      if (!status)
        co_return scada::Event{};
      co_return std::move(event);
    }

    void handle_system_event(const scada::Status& status,
                             const scada::Event& event) {
      if (!status) {
        auto matchers = std::move(matchers_);
        matchers_.clear();
        for (auto& [m, callback] : matchers) {
          callback(status, {});
        }
        return;
      }

      std::vector<callback> matching_callbacks;
      for (auto it = matchers_.begin(); it != matchers_.end();) {
        if (it->first(event)) {
          matching_callbacks.emplace_back(std::move(it->second));
          it = matchers_.erase(it);
        } else {
          ++it;
        }
      }

      for (auto& callback : matching_callbacks) {
        callback(StatusCode::Good, event);
      }
    }

    using matcher = std::function<bool(const scada::Event& event)>;
    using callback = std::function<void(scada::Status, scada::Event)>;
    std::vector<std::pair<matcher, callback>> matchers_;
  };

  const std::shared_ptr<state> state_ = std::make_shared<state>();
  scada::monitored_item monitored_item_;
};

}  // namespace scada
