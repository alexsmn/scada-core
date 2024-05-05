#pragma once

#include "base/awaitable.h"
#include "base/promise.h"

#include <boost/asio/co_spawn.hpp>

template <class E, class T>
inline promise<T> ToPromise(const E& executor, Awaitable<T> aw) {
  promise<T> p;
  boost::asio::co_spawn(executor, std::move(aw),
                        [p](std::exception_ptr e, T result) mutable {
                          if (e) {
                            p.reject(e);
                          } else {
                            p.resolve(result);
                          }
                        });
  return p;
}

template <class E>
inline promise<void> ToPromise(const E& executor, Awaitable<void> aw) {
  promise<void> p;
  boost::asio::co_spawn(executor, std::move(aw),
                        [p](std::exception_ptr e) mutable {
                          if (e) {
                            p.reject(e);
                          } else {
                            p.resolve();
                          }
                        });
  return p;
}

template <class CompletitionHandler = boost::asio::use_awaitable_t<>>
inline auto ToAwaitable(promise<void> p, CompletitionHandler&& handler = {}) {
  auto initiate = [p]<typename Handler>(Handler&& self) mutable {
    p.then([self = std::make_shared<Handler>(std::forward<Handler>(self))](
               /*result*/) { (*self)(); });
  };
  return boost::asio::async_initiate<CompletitionHandler, void(/*result*/)>(
      initiate, handler);
}