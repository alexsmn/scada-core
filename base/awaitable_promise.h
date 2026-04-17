#pragma once

#include "base/awaitable.h"
#include "base/executor.h"
#include "base/promise.h"

#include <boost/asio/async_result.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/post.hpp>
#include <memory>

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

template <class T>
struct PromiseAwaitableResult {
  std::exception_ptr error;
  std::shared_ptr<T> value;
};

struct PromiseAwaitableVoidResult {
  std::exception_ptr error;
};

template <class E, class T, class CompletionToken = boost::asio::use_awaitable_t<>>
inline auto PromiseToAwaitable(E executor,
                               promise<T> p,
                               CompletionToken&& token = {}) {
  auto initiate = [executor = std::move(executor), p = std::move(p)]<
                      typename Handler>(Handler&& handler) mutable {
    auto completion =
        std::make_shared<std::decay_t<Handler>>(std::forward<Handler>(handler));

    p.then([executor, completion](const T& result) mutable {
         boost::asio::post(executor, [completion, result]() mutable {
           (*completion)(PromiseAwaitableResult<T>{
               .error = std::exception_ptr{},
               .value = std::make_shared<T>(result)});
         });
       })
        .except([executor, completion](std::exception_ptr e) mutable {
          boost::asio::post(executor, [completion, e]() mutable {
            (*completion)(PromiseAwaitableResult<T>{.error = e});
          });
        });
  };

  return boost::asio::async_initiate<CompletionToken,
                                     void(PromiseAwaitableResult<T>)>(
      initiate, token);
}

template <class E, class CompletionToken = boost::asio::use_awaitable_t<>>
inline auto PromiseToAwaitable(E executor,
                               promise<void> p,
                               CompletionToken&& token = {}) {
  auto initiate = [executor = std::move(executor), p = std::move(p)]<
                      typename Handler>(Handler&& handler) mutable {
    auto completion =
        std::make_shared<std::decay_t<Handler>>(std::forward<Handler>(handler));

    p.then([executor, completion]() mutable {
         boost::asio::post(executor, [completion]() mutable {
           (*completion)(PromiseAwaitableVoidResult{});
         });
       })
        .except([executor, completion](std::exception_ptr e) mutable {
          boost::asio::post(executor, [completion, e]() mutable {
            (*completion)(PromiseAwaitableVoidResult{.error = e});
          });
        });
  };

  return boost::asio::async_initiate<CompletionToken,
                                     void(PromiseAwaitableVoidResult)>(
      initiate, token);
}

template <class E, class T>
inline Awaitable<T> AwaitPromise(E executor, promise<T> p) {
  auto result = co_await PromiseToAwaitable(std::move(executor), std::move(p));
  if (result.error)
    std::rethrow_exception(result.error);
  co_return std::move(*result.value);
}

template <class E>
inline Awaitable<void> AwaitPromise(E executor, promise<void> p) {
  auto result = co_await PromiseToAwaitable(std::move(executor), std::move(p));
  if (result.error)
    std::rethrow_exception(result.error);
}

template <class CompletitionHandler = boost::asio::use_awaitable_t<>>
inline auto ToAwaitable(promise<void> p, CompletitionHandler&& handler = {}) {
  auto initiate = [p]<typename Handler>(Handler&& self) mutable {
    p.then([self = std::make_shared<Handler>(std::forward<Handler>(self))]() {
       (*self)();
    });
  };
  return boost::asio::async_initiate<CompletitionHandler, void(/*result*/)>(
      initiate, handler);
}
