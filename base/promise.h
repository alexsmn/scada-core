#pragma once

#include <functional>
#include <mutex>
#include <optional>

template <class Result>
class Promise {
 public:
  template <class T>
  void SetResult(T&& result) const {
    std::unique_lock<std::mutex> lock(state_->mutex);
    assert(!state_->result.has_value());
    if (state_->callback) {
      auto callback = std::move(state_->callback);
      state_->callback = nullptr;
      lock.unlock();
      callback(std::move(result));
    } else {
      state_->result = std::forward<T>(result);
    }
  }

  template <class T>
  void Then(T&& callback) const {
    std::unique_lock<std::mutex> lock(state_->mutex);
    assert(!state_->callback);
    if (state_->result.has_value()) {
      auto result = std::move(*state_->result);
      state_->result = std::nullopt;
      lock.unlock();
      callback(std::move(result));
    } else {
      state_->callback = std::forward<T>(callback);
    }
  }

  template <class Lambda>
  static Promise MakeLambda(Lambda&& lambda) {
    Promise promise;
    lambda(promise);
    return promise;
  }

  template <class T>
  static Promise MakeResolved(T&& result) {
    Promise promise;
    promise.SetResult(std::forward<T>(result));
    return promise;
  }

  static Promise<std::vector<Result>> All(base::span<Promise> promises) {
    struct SharedState {
      explicit SharedState(size_t size) : results(size) {}
      ~SharedState() { result_promise.SetResult(std::move(results)); }

      std::vector<Result> results;
      Promise<std::vector<Result>> result_promise;
    };

    auto shared_state = std::make_shared<SharedState>(promises.size());

    for (size_t i = 0; i < promises.size(); ++i) {
      promises[i].Then([i, shared_state](Result&& result) {
        shared_state->results[i] = result;
      });
    }

    return shared_state->result_promise;
  }

 private:
  using Callback = std::function<void(Result&& result)>;

  struct State {
    std::mutex mutex;
    std::optional<Result> result;
    Callback callback;
  };

  const std::shared_ptr<State> state_ = std::make_shared<State>();
};
