#include "remote/protocol_message_transport.h"

#include "base/auto_reset.h"
#include "remote/protocol_buffer.h"

#include <array>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <net/transport_string.h>
#include <net/transport_util.h>

namespace {

net::awaitable<net::ErrorOr<size_t>> ReadPayloadSize(
    net::Transport& transport) {
  std::array<char, protocol::kHeaderSize> header;
  auto bytes_read = co_await net::Read(transport, header);

  if (!bytes_read.ok() || *bytes_read == 0) {
    co_return bytes_read;
  }

  co_return protocol::GetIncomingMessageSize(header) - protocol::kHeaderSize;
}

}  // namespace

ProtocolMessageTransport::ProtocolMessageTransport(
    std::unique_ptr<net::Transport> transport)
    : transport_{std::move(transport)} {
  assert(!transport_->IsMessageOriented());
}

ProtocolMessageTransport::~ProtocolMessageTransport() = default;

net::awaitable<net::Error> ProtocolMessageTransport::Open() {
  co_return co_await transport_->Open();
}

void ProtocolMessageTransport::Close() {
  if (transport_) {
    transport_->Close();
  }
}

net::awaitable<net::ErrorOr<std::unique_ptr<net::Transport>>>
ProtocolMessageTransport::Accept() {
  co_return net::ERR_NOT_IMPLEMENTED;
}

net::awaitable<net::ErrorOr<size_t>> ProtocolMessageTransport::Read(
    std::span<char> data) {
  assert(transport_);
  assert(!transport_->IsMessageOriented());

  if (reading_) {
    co_return net::ERR_IO_PENDING;
  }

  base::AutoReset reading{&reading_, true};

  auto payload_size = co_await ReadPayloadSize(*transport_);
  if (!payload_size.ok() || *payload_size == 0) {
    co_return payload_size;
  }

  co_return co_await net::Read(*transport_, data.subspan(0, *payload_size));
}

net::awaitable<net::ErrorOr<size_t>> ProtocolMessageTransport::Write(
    std::span<const char> data) {
  std::string message;
  protocol::PrependMessageSize(message);
  message.insert(message.end(), data.begin(), data.end());
  protocol::UpdateMessageSize(message);

  auto bytes_written = co_await transport_->Write(message);

  if (!bytes_written.ok()) {
    co_return bytes_written.error();
  }

  // Per transport (and ASIO) specs, bytes written is always equal to the size
  // of the message.
  assert(*bytes_written == message.size());

  // Return the size of the payload.
  co_return data.size();
}

std::string ProtocolMessageTransport::GetName() const {
  return transport_->GetName();
}
