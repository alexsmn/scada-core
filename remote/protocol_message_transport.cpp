#include "remote/protocol_message_transport.h"

#include "base/auto_reset.h"
#include "base/boost_log.h"
#include "remote/protocol_buffer.h"

#include <array>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <transport/transport_util.h>
#include <transport/transport_string.h>

namespace {

BoostLogger logger{LOG_NAME("ProtocolMessageTransport")};

// Reads exactly `data.size()` bytes from the transport.
transport::awaitable<transport::expected<size_t>> ReadExact(
    const transport::any_transport& transport,
    std::span<char> data) {
  size_t total_read = 0;
  while (total_read < data.size()) {
    auto bytes_read = co_await transport.read(data.subspan(total_read));
    if (!bytes_read.ok()) {
      co_return bytes_read.error();
    }
    if (*bytes_read == 0) {
      co_return total_read;
    }
    total_read += *bytes_read;
  }
  co_return total_read;
}

transport::awaitable<transport::expected<size_t>> ReadPayloadSize(
    const transport::any_transport& transport) {
  std::array<char, protocol::kHeaderSize> header{};
  auto bytes_read = co_await ReadExact(transport, header);

  if (!bytes_read.ok() || *bytes_read == 0) {
    co_return bytes_read;
  }

  co_return protocol::GetIncomingMessageSize(header) - protocol::kHeaderSize;
}

}  // namespace

ProtocolMessageTransport::ProtocolMessageTransport(
    transport::any_transport transport)
    : transport_{std::move(transport)} {
  assert(!transport_.message_oriented());
}

ProtocolMessageTransport::~ProtocolMessageTransport() = default;

transport::awaitable<transport::error_code> ProtocolMessageTransport::open() {
  co_return co_await transport_.open();
}

transport::awaitable<transport::error_code> ProtocolMessageTransport::close() {
  if (transport_) {
    co_return co_await transport_.close();
  }
  co_return transport::OK;
}

transport::awaitable<transport::expected<transport::any_transport>>
ProtocolMessageTransport::accept() {
  co_return transport::ERR_NOT_IMPLEMENTED;
}

transport::awaitable<transport::expected<size_t>>
ProtocolMessageTransport::read(std::span<char> data) {
  assert(transport_);
  assert(!transport_.message_oriented());

  if (reading_) {
    co_return transport::ERR_IO_PENDING;
  }

  base::AutoReset reading{&reading_, true};

  auto payload_size = co_await ReadPayloadSize(transport_);
  if (!payload_size.ok() || *payload_size == 0) {
    LOG_INFO(logger) << "Read payload size failed or closed"
                     << LOG_TAG("Ok", payload_size.ok())
                     << LOG_TAG("Value", payload_size.ok() ? *payload_size : 0);
    co_return payload_size;
  }

  LOG_INFO(logger) << "Read payload"
                   << LOG_TAG("PayloadSize", *payload_size);

  auto payload_read = co_await ReadExact(transport_, data.subspan(0, *payload_size));
  LOG_INFO(logger) << "Read payload completed"
                   << LOG_TAG("Ok", payload_read.ok())
                   << LOG_TAG("BytesRead", payload_read.ok() ? *payload_read : 0);
  co_return payload_read;
}

transport::awaitable<transport::expected<size_t>>
ProtocolMessageTransport::write(std::span<const char> data) {
  LOG_INFO(logger) << "Write payload" << LOG_TAG("PayloadSize", data.size());
  std::string message;
  protocol::PrependMessageSize(message);
  message.insert(message.end(), data.begin(), data.end());
  protocol::UpdateMessageSize(message);

  auto write_error = co_await transport::Write(
      transport_, std::span<char>{message.data(), message.size()});

  if (write_error != transport::OK) {
    LOG_WARNING(logger) << "Write failed";
    co_return write_error;
  }

  LOG_INFO(logger) << "Write payload completed"
                   << LOG_TAG("PayloadSize", data.size());

  // Return the size of the payload.
  co_return data.size();
}

std::string ProtocolMessageTransport::name() const {
  return transport_.name();
}
