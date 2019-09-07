#pragma once

#include <functional>
#include <memory>
#include <optional>
#include <vector>

template <class Topic, class Executor>
class Subscription
    : public std::enable_shared_from_this<Subscription<Topic, Executor>> {
 public:
  typedef Topic Topic;
  using Version = unsigned;
  using Callback = std::function<void(Version version, const Topic& topic)>;

  explicit Subscription(Executor& executor) : executor_{executor} {}

  void Request(std::optional<Version> version, const Callback& callback) {
    executor_.dispatch([ref = shared_from_this(), this, version, callback] {
      if (version.has_value() && version_.has_value() && *version < *version_)
        callback(*version_, *topic_);
      else
        subscriptions_.emplace_back(version, callback);
    });
  }

  void Update(const Topic& topic) {
    executor_.dispatch([ref = shared_from_this(), this, topic] {
      if (version_)
        ++*version_;
      else
        version_ = 1;

      topic_ = topic;

      // TODO: Check version.
      auto subscriptions = std::move(subscriptions_);
      subscriptions_.clear();

      for (const auto& subscription : subscriptions)
        subscription.second(*version_, *topic_);
    });
  }

 private:
  Executor& executor_;

  std::optional<Version> version_;
  std::optional<Topic> topic_;

  mutable std::vector<std::pair<std::optional<Version>, Callback>>
      subscriptions_;
};
