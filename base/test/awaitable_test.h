#pragma once

#include "base/awaitable.h"
#include "base/promise.h"
#include "base/test/test_executor.h"

#include <thread>

using namespace std::chrono_literals;

inline void Drain(const std::shared_ptr<TestExecutor>& executor) {
  while (executor->GetTaskCount() > 0) {
    executor->Poll();
  }
}

inline void WaitPromise(std::shared_ptr<TestExecutor> executor,
                        promise<void> promise) {
  while (promise.wait_for(0ms) == promise_wait_status::timeout) {
    Drain(executor);
    std::this_thread::yield();
  }

  promise.get();
}

template <class T>
T WaitPromise(std::shared_ptr<TestExecutor> executor, promise<T> promise) {
  while (promise.wait_for(0ms) == promise_wait_status::timeout) {
    Drain(executor);
    std::this_thread::yield();
  }

  return promise.get();
}

template <class T>
T WaitPromise(std::shared_ptr<TestExecutor> executor,
              std::shared_ptr<promise<T>> promise) {
  while (promise->wait_for(0ms) == promise_wait_status::timeout) {
    Drain(executor);
    std::this_thread::yield();
  }

  return promise->get();
}

inline void WaitPromise(std::shared_ptr<TestExecutor> executor,
                        std::shared_ptr<promise<void>> promise) {
  while (promise->wait_for(0ms) == promise_wait_status::timeout) {
    Drain(executor);
    std::this_thread::yield();
  }

  promise->get();
}

template <class T>
T WaitAwaitable(std::shared_ptr<TestExecutor> executor, Awaitable<T> awaitable) {
  using PromiseT = promise<T>;
  std::shared_ptr<PromiseT> promise{new PromiseT()};
  CoSpawn(MakeTestAnyExecutor(executor),
          [promise, awaitable = std::move(awaitable)]() mutable
              -> Awaitable<void> {
            try {
              promise->resolve(co_await std::move(awaitable));
            } catch (...) {
              promise->reject(std::current_exception());
            }
          });
  return WaitPromise(executor, std::move(promise));
}

inline void WaitAwaitable(std::shared_ptr<TestExecutor> executor,
                          Awaitable<void> awaitable) {
  using PromiseT = promise<void>;
  std::shared_ptr<PromiseT> promise{new PromiseT()};
  CoSpawn(MakeTestAnyExecutor(executor),
          [promise, awaitable = std::move(awaitable)]() mutable
              -> Awaitable<void> {
            try {
              co_await std::move(awaitable);
              promise->resolve();
            } catch (...) {
              promise->reject(std::current_exception());
            }
          });
  WaitPromise(executor, std::move(promise));
}
