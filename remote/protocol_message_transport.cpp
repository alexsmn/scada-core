#include "remote/protocol_message_transport.h"

#include "base/boost_log.h"
#include "base/check.h"
#include "remote/protocol_buffer.h"

#include <array>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <transport/transport_string.h>
#include <transport/transport_util.h>

namespace {

BoostLogger logger{LOG_NAME("ProtocolMessageTransport")};

// Reads exactly `data.size()` bytes from the transport. `transport` is a
// reference into the owning ProtocolMessageTransport, which may be destroyed
// while a child read is suspended (session teardown resets the transport with
// the read loop in flight); `cancelation` expires with the owner, so every
// resumption must check it before touching `transport` again.
transport::awaitable<transport::expected<size_t>> ReadExact(
    const transport::any_transport& transport,
    std::weak_ptr<bool> cancelation,
    std::span<char> data) {
  size_t total_read = 0;
  while (total_read < data.size()) {
    auto bytes_read = co_await transport.read(data.subspan(total_read));
    if (cancelation.expired()) {
      co_return transport::ERR_ABORTED;
    }
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
    const transport::any_transport& transport,
    std::weak_ptr<bool> cancelation) {
  std::array<char, protocol::kHeaderSize> header{};
  auto bytes_read =
      co_await ReadExact(transport, std::move(cancelation), header);

  if (!bytes_read.ok() || *bytes_read == 0) {
    co_return bytes_read;
  }

  co_return protocol::GetIncomingMessageSize(header) - protocol::kHeaderSize;
}

}  // namespace

ProtocolMessageTransport::ProtocolMessageTransport(
    transport::any_transport transport)
    : transport_{std::move(transport)} {
  scada::base::Check(!transport_.message_oriented());
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
  scada::base::Check(transport_);
  scada::base::Check(!transport_.message_oriented());

  if (reading_) {
    co_return transport::ERR_IO_PENDING;
  }

  // Session teardown destroys `*this` while the child reads below are
  // suspended, and the frames only resume when the aborted socket read
  // completes. `cancelation_` expires together with `*this`, so every
  // resumption checks it before touching members. That is also why `reading_`
  // is managed manually rather than with base::AutoReset: the AutoReset
  // destructor would write to the freed member on the aborted path.
  std::weak_ptr<bool> cancelation = cancelation_;
  reading_ = true;

  auto payload_size = co_await ReadPayloadSize(transport_, cancelation);
  if (cancelation.expired()) {
    co_return transport::ERR_ABORTED;
  }
  if (!payload_size.ok() || *payload_size == 0) {
    reading_ = false;
    LOG_INFO(logger) << "Read payload size failed or closed"
                     << LOG_TAG("Ok", payload_size.ok())
                     << LOG_TAG("Value", payload_size.ok() ? *payload_size : 0);
    co_return payload_size;
  }

  // The payload size comes from the wire; reject anything larger than the
  // caller's buffer instead of forming an out-of-range subspan.
  if (*payload_size > data.size()) {
    reading_ = false;
    LOG_ERROR(logger) << "Incoming message exceeds read buffer"
                      << LOG_TAG("PayloadSize", *payload_size)
                      << LOG_TAG("BufferSize", data.size());
    co_return transport::ERR_FAILED;
  }

  LOG_DEBUG(logger) << "Read payload" << LOG_TAG("PayloadSize", *payload_size);

  auto payload_read = co_await ReadExact(transport_, cancelation,
                                         data.subspan(0, *payload_size));
  if (cancelation.expired()) {
    co_return transport::ERR_ABORTED;
  }
  reading_ = false;
  LOG_DEBUG(logger) << "Read payload completed"
                    << LOG_TAG("Ok", payload_read.ok())
                    << LOG_TAG("BytesRead",
                               payload_read.ok() ? *payload_read : 0);
  co_return payload_read;
}

transport::awaitable<transport::expected<size_t>>
ProtocolMessageTransport::write(std::span<const char> data) {
  LOG_DEBUG(logger) << "Write payload" << LOG_TAG("PayloadSize", data.size());
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

  LOG_DEBUG(logger) << "Write payload completed"
                    << LOG_TAG("PayloadSize", data.size());

  // Return the size of the payload.
  co_return data.size();
}

std::string ProtocolMessageTransport::name() const {
  return transport_.name();
}
