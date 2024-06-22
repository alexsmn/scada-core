#pragma once

#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <net/any_transport.h>

class RemoteListener {
 public:
  using AcceptHandler = std::function<void(net::any_transport transport)>;

  // `listener_name` is used for logging purposes.
  RemoteListener(std::shared_ptr<BoostLogger> logger,
                 net::any_transport transport,
                 std::string listener_name,
                 AcceptHandler accept_handler)
      : logger_{std::move(logger)},
        acceptor_{std::move(transport)},
        listener_name_{std::move(listener_name)},
        accept_handler_{std::move(accept_handler)} {}

  promise<> Init() {
    LOG_INFO(*logger_) << "Listening..." << LOG_TAG("Listener", listener_name_);

    boost::asio::co_spawn(
        acceptor_.get_executor(),
        [this]() -> net::awaitable<void> { OnTransportClosed(co_await Run()); },
        boost::asio::detached);

    return open_promise_;
  }

 private:
  [[nodiscard]] net::awaitable<net::Error> Run() {
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

  void OnTransportClosed(net::Error error) {
    LOG_ERROR(*logger_) << "Listener error"
                        << LOG_TAG("Listener", listener_name_)
                        << LOG_TAG("ErrorString", net::ErrorToString(error));

    open_promise_.reject(std::runtime_error{net::ErrorToString(error)});
  }

  const std::shared_ptr<BoostLogger> logger_;
  net::any_transport acceptor_;
  const std::string listener_name_;
  const AcceptHandler accept_handler_;

  promise<> open_promise_;
};
