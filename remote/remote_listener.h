#pragma once

class RemoteListener {
 public:
  using SessionAcceptedHandler =
      std::function<void(std::unique_ptr<net::Transport> transport)>;

  RemoteListener(std::shared_ptr<BoostLogger> logger,
                 std::unique_ptr<net::Transport> transport,
                 SessionAcceptedHandler session_accepted_handler)
      : logger_{std::move(logger)},
        transport_{std::move(transport)},
        session_accepted_handler_{std::move(session_accepted_handler)} {}

  promise<> Open() {
    LOG_INFO(*logger_) << "Listening..."
                       << LOG_TAG("Listener", transport_->GetName());

    transport_->Open(
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
                       << LOG_TAG("Listener", transport_->GetName());
    open_promise_.resolve();
  }

  void OnTransportClosed(net::Error error) {
    LOG_ERROR(*logger_) << "Listener error"
                        << LOG_TAG("Listener", transport_->GetName())
                        << LOG_TAG("ErrorString", net::ErrorToString(error));
    open_promise_.reject(std::runtime_error{net::ErrorToString(error)});
  }

  net::Error OnTransportAccepted(std::unique_ptr<net::Transport> transport) {
    LOG_INFO(*logger_) << "Client connection accepted"
                       << LOG_TAG("Listener", transport_->GetName())
                       << LOG_TAG("Transport", transport->GetName());
    session_accepted_handler_(std::move(transport));
    return net::OK;
  }

  const std::shared_ptr<BoostLogger> logger_;
  const std::unique_ptr<net::Transport> transport_;
  const SessionAcceptedHandler session_accepted_handler_;

  promise<> open_promise_;
};
