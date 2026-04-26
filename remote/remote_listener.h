#pragma once

#include "base/async_completion.h"
#include "base/awaitable.h"
#include "base/boost_log.h"

#include <exception>
#include <functional>
#include <memory>
#include <stdexcept>
#include <string>
#include <transport/any_transport.h>
#include <transport/expected.h>
#include <utility>

class RemoteListener : public std::enable_shared_from_this<RemoteListener> {
 public:
  using AcceptHandler = std::function<void(transport::any_transport transport)>;

  static std::shared_ptr<RemoteListener> Create(
      std::shared_ptr<BoostLogger> logger,
      transport::any_transport transport,
      std::string listener_name,
      AcceptHandler accept_handler) {
    return std::shared_ptr<RemoteListener>(new RemoteListener(
        std::move(logger), std::move(transport), std::move(listener_name),
        std::move(accept_handler)));
  }

  // `listener_name` is used for logging purposes.
  RemoteListener(std::shared_ptr<BoostLogger> logger,
                 transport::any_transport transport,
                 std::string listener_name,
                 AcceptHandler accept_handler)
      : logger_{std::move(logger)},
        acceptor_{std::move(transport)},
        listener_name_{std::move(listener_name)},
        accept_handler_{std::move(accept_handler)},
        open_completion_{acceptor_.get_executor()},
        close_completion_{acceptor_.get_executor()} {}

  [[nodiscard]] Awaitable<void> InitAsync() {
    LOG_INFO(*logger_) << "Listening..." << LOG_TAG("Listener", listener_name_);

    auto self = shared_from_this();
    CoSpawn(acceptor_.get_executor(), [self]() -> Awaitable<void> {
      try {
        self->OnTransportClosed(co_await self->Run());
      } catch (...) {
        self->OnListenerException(std::current_exception());
      }
      self->close_completion_.Complete();
    });

    co_await open_completion_.Wait();
  }

  [[nodiscard]] Awaitable<void> ShutdownAsync() {
    auto self = shared_from_this();
    CoSpawn(acceptor_.get_executor(), [self]() -> Awaitable<void> {
      [[maybe_unused]] auto result = co_await self->acceptor_.close();
    });

    co_await close_completion_.Wait();
  }

 private:
  [[nodiscard]] transport::awaitable<transport::error_code> Run() {
    NET_CO_RETURN_IF_ERROR(co_await acceptor_.open());

    LOG_INFO(*logger_) << "Listener opened"
                       << LOG_TAG("Listener", listener_name_);

    opened_ = true;
    open_completion_.Complete();

    for (;;) {
      NET_ASSIGN_OR_CO_RETURN(auto transport, co_await acceptor_.accept());

      LOG_INFO(*logger_) << "Client connection accepted"
                         << LOG_TAG("Listener", listener_name_)
                         << LOG_TAG("Transport", transport.name());

      accept_handler_(std::move(transport));
    }
  }

  void OnTransportClosed(transport::error_code error) {
    if (!opened_) {
      LOG_ERROR(*logger_) << "Listener error"
                          << LOG_TAG("Listener", listener_name_)
                          << LOG_TAG("ErrorString",
                                     transport::ErrorToString(error));

      open_completion_.Fail(std::make_exception_ptr(
          std::runtime_error{transport::ErrorToString(error)}));
      return;
    }

    LOG_INFO(*logger_) << "Listener closed"
                       << LOG_TAG("Listener", listener_name_)
                       << LOG_TAG("ErrorString",
                                  transport::ErrorToString(error));
  }

  void OnListenerException(std::exception_ptr error) {
    try {
      std::rethrow_exception(error);
    } catch (const std::exception& ex) {
      LOG_ERROR(*logger_) << "Listener exception"
                          << LOG_TAG("Listener", listener_name_)
                          << LOG_TAG("ErrorString", ex.what());
    } catch (...) {
      LOG_ERROR(*logger_) << "Listener exception"
                          << LOG_TAG("Listener", listener_name_);
    }

    if (!opened_) {
      open_completion_.Fail(std::move(error));
    }
  }

  const std::shared_ptr<BoostLogger> logger_;
  transport::any_transport acceptor_;
  const std::string listener_name_;
  const AcceptHandler accept_handler_;

  base::AsyncCompletion open_completion_;
  base::AsyncCompletion close_completion_;
  bool opened_ = false;
};
