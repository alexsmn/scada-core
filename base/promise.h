#pragma once

#include <promise.hpp/promise.hpp>

using namespace promise_hpp;

template <class T>
struct PromiseResult {
  using type = T;
};

template <class T>
struct PromiseResult<promise<T>> {
  using type = T;
};

template <class T>
using promise_result_t = typename PromiseResult<T>::type;

inline promise<> MakeRejectedPromise() {
  return make_rejected_promise(std::exception{});
}

template <class T>
inline promise<T> MakeRejectedPromise() {
  return make_rejected_promise<T>(std::exception{});
}

template <class T>
inline promise<> ToVoidPromise(promise<T> promise) {
  return promise.then([](const T& value) {});
}

template <class U>
inline promise<U> ToValuePromise(promise<> promise, const U& value) {
  return promise.then([value] { return value; });
}

template <class T, class U>
inline promise<U> ToValuePromise(promise<T> promise, const U& value) {
  return promise.then([value](const T& value) { return value; });
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

inline promise<> ForwardPromise(promise<> source_promise,
                                promise<> target_promise) {
  source_promise.then([target_promise]() mutable { target_promise.resolve(); })
      .except([target_promise](std::exception_ptr e) mutable {
        target_promise.reject(e);
      });
  return target_promise;
}

template <class T>
inline promise<T> ForwardPromise(promise<T> source_promise,
                                 promise<> target_promise) {
  source_promise.then([target_promise]() mutable { target_promise.resolve(); })
      .except([target_promise](std::exception_ptr e) mutable {
        target_promise.reject(e);
      });
  return target_promise;
}

template <class T>
inline promise<T> ForwardPromise(promise<T> source_promise,
                                 promise<T> target_promise) {
  source_promise
      .then([target_promise](const T& value) mutable {
        target_promise.resolve(value);
      })
      .except([target_promise](std::exception_ptr e) mutable {
        target_promise.reject(e);
      });
  return target_promise;
}

inline promise<> FoldPromises(promise<> a, promise<> b) {
  return a.then([b]() mutable { return b; });
}
