#pragma once

#include "base/promise.h"
#include "net/base/net_errors.h"

#include <boost/asio/dispatch.hpp>
#include <boost/asio/executor.hpp>
#include <net/transport.h>
#include <stdexcept>

namespace internal {

class PromiseHandlersException : public std::exception {
 public:
  explicit PromiseHandlersException(net::Error error) : error_(error) {}

  net::Error error() const { return error_; }

 private:
  const net::Error error_;
};

struct PromiseHandlers : std::enable_shared_from_this<PromiseHandlers> {
  PromiseHandlers(net::Connector::OpenHandler on_open,
                  net::Connector::CloseHandler on_close)
      : on_open{std::move(on_open)}, on_close{std::move(on_close)} {}

  void OnOpen() {
    // Keep reference is case the callback is deleted along with the captured
    // `state` instance.
    auto ref = shared_from_this();

    if (on_open) {
      on_open();
    }

    if (!promise_set) {
      promise_set = true;
      promise.resolve();
    }
  }

  void OnClose(net::Error error) {
    // Keep reference is case the callback is deleted along with the captured
    // `state` instance.
    auto ref = shared_from_this();

    if (on_close) {
      on_close(error);
    }

    if (!promise_set) {
      promise_set = true;
      promise.reject(PromiseHandlersException{error});
    }
  }

  net::Transport::OpenHandler on_open;
  net::Transport::CloseHandler on_close;

  bool promise_set = false;
  promise<void> promise;
};

}  // namespace internal

inline std::pair<promise<void>, net::Connector::Handlers> MakePromiseHandlers(
    const net::Connector::Handlers& handlers) {
  auto state = std::make_shared<internal::PromiseHandlers>(handlers.on_open,
                                                           handlers.on_close);

  auto promise_handlers = handlers;

  promise_handlers.on_open =
      std::bind_front(&internal::PromiseHandlers::OnOpen, state);

  promise_handlers.on_close =
      std::bind_front(&internal::PromiseHandlers::OnClose, state);

  return {state->promise, std::move(promise_handlers)};
}

template <typename E, typename F>
inline promise<void> DispatchAsPromise(const E& ex, F&& f) {
  promise<void> p;
  boost::asio::dispatch(ex, [p, f = std::forward<F>(f)]() mutable {
    f().then([p]() mutable { p.resolve(); },
             [p](std::exception_ptr e) mutable { p.reject(e); });
  });
  return p;
}
