#pragma once

#include "scada/client.h"
#include "scada/status_promise.h"

namespace scada {

struct event_awaiter {
  explicit event_awaiter(const scada::node& node) {
    monitored_item_.subscribe_system_events(
        node,
        /*params*/ {}, std::bind_front(&state::handle_system_event, state_));
  }

  template <class T>
  status_promise<scada::Event> when(T&& matcher) {
    return state_->when(std::forward<T>(matcher));
  }

  status_promise<scada::Event> when_any() {
    return state_->when([](const scada::Event& event) { return true; });
  }

 private:
  struct state {
    template <class T>
    status_promise<scada::Event> when(T&& matcher) {
      return matchers_
          .emplace_back(std::forward<T>(matcher), promise<scada::Event>{})
          .second;
    }

    void handle_system_event(const scada::Status& status,
                             const scada::Event& event) {
      if (!status) {
        for (auto& [m, p] : matchers_) {
          scada::RejectStatusPromise(p, status);
        }
        matchers_.clear();
        return;
      }

      auto matching = std::ranges::partition(
          matchers_, [&event](const auto& m) { return !m(event); },
          [](const auto& p) { return p.first; });

      for (auto& [m, p] : matching) {
        p.resolve(event);
      }

      matchers_.erase(matching.begin(), matching.end());
    }

    using matcher = std::function<bool(const scada::Event& event)>;
    std::vector<std::pair<matcher, status_promise<scada::Event>>> matchers_;
  };

  const std::shared_ptr<state> state_ = std::make_shared<state>();
  scada::monitored_item monitored_item_;
};

}  // namespace scada
