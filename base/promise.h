#pragma once

#include "base/executor.h"

#include <promise.hpp/promise.hpp>

using namespace promise_hpp;

inline promise<> MakeRejectedPromise() {
  return make_rejected_promise(std::exception{});
}

template <class T>
inline promise<> ToVoidPromise(promise<T> promise) {
  return promise.then([](const T& value) {});
}

template <class T>
inline promise<> ToRejectedPromise(promise<T> promise) {
  return promise.then(
      [](const T& value) { return make_rejected_promise(std::exception{}); });
}

template <class R, class T>
inline promise<R> ToRejectedPromise(promise<T> promise) {
  return promise.then([](const T& value) {
    return make_rejected_promise<R>(std::exception{});
  });
}
