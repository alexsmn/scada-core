#pragma once

#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <memory>
#include <transport/any_transport.h>
#include <transport/expected.h>

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
        accept_handler_{std::move(accept_handler)} {}

  promise<> Init() {
    LOG_INFO(*logger_) << "Listening..." << LOG_TAG("Listener", listener_name_);

    auto self = shared_from_this();
    boost::asio::co_spawn(
        acceptor_.get_executor(),
        [self]() -> transport::awaitable<void> {
          self->OnTransportClosed(co_await self->Run());
        },
        boost::asio::detached);

    return open_promise_;
  }

 private:
  [[nodiscard]] transport::awaitable<transport::error_code> Run() {
    NET_CO_RETURN_IF_ERROR(co_await acceptor_.open());

    LOG_INFO(*logger_) << "Listener opened"
                       << LOG_TAG("Listener", listener_name_);

    open_promise_.resolve();

    for (;;) {
      NET_ASSIGN_OR_CO_RETURN(auto transport, co_await acceptor_.accept());

      LOG_INFO(*logger_) << "Client connection accepted"
                         << LOG_TAG("Listener", listener_name_)
                         << LOG_TAG("Transport", transport.name());

      accept_handler_(std::move(transport));
    }
  }

  void OnTransportClosed(transport::error_code error) {
    LOG_ERROR(*logger_) << "Listener error"
                        << LOG_TAG("Listener", listener_name_)
                        << LOG_TAG("ErrorString",
                                   transport::ErrorToString(error));

    open_promise_.reject(
        std::runtime_error{transport::ErrorToString(error)});
  }

  const std::shared_ptr<BoostLogger> logger_;
  transport::any_transport acceptor_;
  const std::string listener_name_;
  const AcceptHandler accept_handler_;

  promise<> open_promise_;
};
