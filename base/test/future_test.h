#pragma once

#include <chrono>
#include <future>
#include <stdexcept>

template <class T, class Rep, class Period>
T WaitFuture(std::future<T>& future,
             std::chrono::duration<Rep, Period> timeout) {
  if (future.wait_for(timeout) != std::future_status::ready) {
    throw std::runtime_error{"Timed out waiting for future"};
  }
  return future.get();
}

template <class T>
T WaitFuture(std::future<T>& future) {
  return WaitFuture(future, std::chrono::seconds{1});
}
