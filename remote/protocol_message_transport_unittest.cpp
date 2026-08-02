#include "remote/protocol_message_transport.h"

#include "base/test/asio_test_environment.h"
#include "remote/protocol_buffer.h"

#include <transport/any_transport.h>
#include <transport/transport.h>

#include <gtest/gtest.h>

#include <algorithm>
#include <memory>
#include <span>
#include <string>
#include <vector>

namespace {

// A read-only fake transport that hands out a preloaded byte stream over
// successive read() calls, so ProtocolMessageTransport can reassemble one framed
// message from it. Non-message-oriented, as the wrapped transport must be.
class ByteStreamTransport final : public transport::Transport {
 public:
  ByteStreamTransport(transport::executor executor, std::string data)
      : executor_{std::move(executor)}, data_{std::move(data)} {}

  transport::awaitable<transport::error_code> open() override {
    co_return transport::OK;
  }
  transport::awaitable<transport::error_code> close() override {
    co_return transport::OK;
  }
  transport::awaitable<transport::expected<transport::any_transport>> accept()
      override {
    co_return transport::ERR_NOT_IMPLEMENTED;
  }
  transport::awaitable<transport::expected<size_t>> read(
      std::span<char> data) override {
    const size_t n = std::min(data.size(), data_.size() - offset_);
    std::copy_n(data_.begin() + offset_, n, data.begin());
    offset_ += n;
    co_return n;
  }
  transport::awaitable<transport::expected<size_t>> write(
      std::span<const char> data) override {
    co_return data.size();
  }
  std::string name() const override { return "ByteStream"; }
  bool message_oriented() const override { return false; }
  bool connected() const override { return true; }
  bool active() const override { return true; }
  transport::executor get_executor() override { return executor_; }

 private:
  transport::executor executor_;
  std::string data_;
  size_t offset_ = 0;
};

// Frames a payload of `payload_size` bytes the way write() does: a
// MessageSizeType length header followed by the payload.
std::string FrameMessage(size_t payload_size) {
  std::string framed;
  protocol::PrependMessageSize(framed);
  framed.append(payload_size, 'x');
  protocol::UpdateMessageSize(framed);
  return framed;
}

std::unique_ptr<ProtocolMessageTransport> MakeTransport(
    AsioTestEnvironment& env,
    std::string framed) {
  return std::make_unique<ProtocolMessageTransport>(transport::any_transport{
      std::make_unique<ByteStreamTransport>(env.any_executor_factory(),
                                             std::move(framed))});
}

// A payload larger than the historical fixed 1 MiB read buffer whose overflow
// aborted the session mid-load (client/server E2E ExpandsObjectTreeLabels), but
// within kMaxMessageSize.
constexpr size_t kLargePayload = 2 * 1024 * 1024;

// Regression: a message larger than the former 1 MiB buffer reassembles when the
// caller's buffer is sized to kMaxMessageSize.
TEST(ProtocolMessageTransportTest, ReadsMessageLargerThanLegacyBuffer) {
  AsioTestEnvironment env;
  auto transport = MakeTransport(env, FrameMessage(kLargePayload));

  std::vector<char> buffer(protocol::kMaxMessageSize);
  auto result = env.Wait(transport->read(buffer));

  ASSERT_TRUE(result.ok());
  EXPECT_EQ(*result, kLargePayload);
}

// The size guard still holds: a payload exceeding the caller's buffer is
// rejected rather than overflowing (the former 1 MiB buffer would have hit this
// on the 2 MiB message).
TEST(ProtocolMessageTransportTest, RejectsMessageLargerThanBuffer) {
  AsioTestEnvironment env;
  auto transport = MakeTransport(env, FrameMessage(kLargePayload));

  std::vector<char> buffer(1024 * 1024);
  auto result = env.Wait(transport->read(buffer));

  EXPECT_FALSE(result.ok());
}

}  // namespace
