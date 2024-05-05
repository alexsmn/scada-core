#include "remote/protocol_message_transport.h"

#include "remote/protocol_buffer.h"

#include <net/transport_string.h>

ProtocolMessageTransport::ProtocolMessageTransport(
    std::unique_ptr<net::Transport> transport)
    : transport_{std::move(transport)} {
  assert(!transport_->IsMessageOriented());
}

ProtocolMessageTransport::~ProtocolMessageTransport() = default;

net::awaitable<void> ProtocolMessageTransport::Open(Handlers handlers) {
  handlers_ = std::move(handlers);

  return transport_->Open(
      {.on_open = [this] { OnTransportOpened(); },
       .on_close = [this](net::Error error) { OnTransportClosed(error); },
       .on_data = [this] { OnTransportDataReceived(); }});
}

void ProtocolMessageTransport::Close() {
  handlers_ = {};

  if (transport_)
    transport_->Close();
}

int ProtocolMessageTransport::Read(std::span<char> data) {
  return net::ERR_ABORTED;
}

net::awaitable<size_t> ProtocolMessageTransport::Write(std::vector<char> data) {
  std::string message;
  protocol::PrependMessageSize(message);
  message.insert(message.end(), data.begin(), data.end());
  protocol::UpdateMessageSize(message);
  return transport_->Write(std::vector<char>{message.begin(), message.end()});
}

std::string ProtocolMessageTransport::GetName() const {
  return transport_->GetName();
}

void ProtocolMessageTransport::OnTransportOpened() {
  if (handlers_.on_open) {
    handlers_.on_open();
  }
}

void ProtocolMessageTransport::OnTransportClosed(net::Error error) {
  if (handlers_.on_close) {
    handlers_.on_close(error);
  }
}

void ProtocolMessageTransport::OnTransportDataReceived() {
  std::weak_ptr<bool> cancelation = cancelation_;
  while (!cancelation.expired()) {
    while (incoming_message_.size() <
           protocol::GetIncomingMessageSize(incoming_message_)) {
      auto original_size = incoming_message_.size();
      auto size = protocol::GetIncomingMessageSize(incoming_message_);
      incoming_message_.resize(size);

      auto read_count = transport_->Read(
          {&incoming_message_[original_size], size - original_size});

      if (read_count < 0) {
        if (handlers_.on_close) {
          handlers_.on_close(static_cast<net::Error>(read_count));
        }
        return;
      }

      incoming_message_.resize(original_size + read_count);
      if (incoming_message_.size() < size) {
        return;
      }
    }

    auto incoming_message = std::move(incoming_message_);
    incoming_message_.clear();

    if (handlers_.on_message) {
      handlers_.on_message({static_cast<const char*>(
                                protocol::GetMessagePayload(incoming_message)),
                            protocol::GetMessagePayloadSize(incoming_message)});
    }
  }
}
