#pragma once

#include <net/acceptor.h>

class RemoteListener {
 public:
  using SessionAcceptedHandler =
      std::function<void(std::unique_ptr<net::Transport> transport)>;

  // `listener_name` is used for logging purposes.
  RemoteListener(std::shared_ptr<BoostLogger> logger,
                 net::acceptor acceptor,
                 std::string listener_name,
                 SessionAcceptedHandler session_accepted_handler)
      : logger_{std::move(logger)},
        acceptor_{std::move(acceptor)},
        listener_name_{std::move(listener_name)},
        session_accepted_handler_{std::move(session_accepted_handler)} {}

  promise<> Init() {
    LOG_INFO(*logger_) << "Listening..." << LOG_TAG("Listener", listener_name_);

    acceptor_.open(
        {.on_open = [this] { OnTransportOpened(); },
         .on_close = [this](net::Error error) { OnTransportClosed(error); },
         .on_accept =
             [this](std::unique_ptr<net::Transport> transport) {
               return OnTransportAccepted(std::move(transport));
             }});

    return open_promise_;
  }

 private:
  void OnTransportOpened() {
    LOG_INFO(*logger_) << "Listener opened"
                       << LOG_TAG("Listener", listener_name_);

    open_promise_.resolve();
  }

  void OnTransportClosed(net::Error error) {
    LOG_ERROR(*logger_) << "Listener error"
                        << LOG_TAG("Listener", listener_name_)
                        << LOG_TAG("ErrorString", net::ErrorToString(error));

    open_promise_.reject(std::runtime_error{net::ErrorToString(error)});
  }

  net::Error OnTransportAccepted(std::unique_ptr<net::Transport> transport) {
    LOG_INFO(*logger_) << "Client connection accepted"
                       << LOG_TAG("Listener", listener_name_)
                       << LOG_TAG("Transport", transport->GetName());

    session_accepted_handler_(std::move(transport));
    return net::OK;
  }

  const std::shared_ptr<BoostLogger> logger_;
  net::acceptor acceptor_;
  const std::string listener_name_;
  const SessionAcceptedHandler session_accepted_handler_;

  promise<> open_promise_;
};
