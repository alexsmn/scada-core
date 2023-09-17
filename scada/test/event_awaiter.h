#pragma once

#include "scada/client.h"

namespace scada {

struct event_awaiter {
  explicit event_awaiter(const scada::node& node)
      : monitored_item_{node.subscribe_system_events(
            /*filter*/ {},
            std::bind_front(&state::handle_system_event, state_))} {
    if (!monitored_item_.subscribed()) {
      state_->set_subscription_failed();
    }
  }

  template <class T>
  promise<scada::Event> when(T&& matcher) {
    return state_->when(std::forward<T>(matcher));
  }

  promise<scada::Event> when_any() {
    return state_->when([](const scada::Event& event) { return true; });
  }

 private:
  struct state {
    void set_subscription_failed() {
      subscription_failed_ = true;

      for (auto& [m, p] : matchers_) {
        scada::RejectStatusPromise(p, scada::StatusCode::Bad_Disconnected);
      }
    }

    template <class T>
    promise<scada::Event> when(T&& matcher) {
      if (subscription_failed_) {
        return scada::MakeRejectedStatusPromise<scada::Event>(
            scada::StatusCode::Bad_Disconnected);
      }

      return matchers_
          .emplace_back(std::forward<T>(matcher), promise<scada::Event>{})
          .second;
    }

    void handle_system_event(const scada::Status& status,
                             const scada::Event& event) {
      auto matching = std::ranges::partition(
          matchers_, [&event](const auto& m) { return !m(event); },
          [](const auto& p) { return p.first; });

      for (auto& [m, p] : matching) {
        p.resolve(event);
      }

      matchers_.erase(matching.begin(), matching.end());
    }

    bool subscription_failed_ = false;

    using matcher = std::function<bool(const scada::Event& event)>;
    std::vector<std::pair<matcher, promise<scada::Event>>> matchers_;
  };

  const std::shared_ptr<state> state_ = std::make_shared<state>();
  const scada::monitored_item monitored_item_;
};

}  // namespace scada
