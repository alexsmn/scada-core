#pragma once

#include <queue>
#include <string>

#include "transport/any_transport.h"
#include "transport/transport.h"

// Operates with unframed messages. Automatically adds frames on send and
// removes on reception.
class ProtocolMessageTransport final : public transport::Transport {
 public:
  explicit ProtocolMessageTransport(transport::any_transport transport);
  ~ProtocolMessageTransport();

  [[nodiscard]] virtual transport::awaitable<transport::error_code>
  open() override;

  [[nodiscard]] virtual transport::awaitable<transport::error_code>
  close() override;

  [[nodiscard]] virtual transport::awaitable<
      transport::expected<transport::any_transport>>
  accept() override;

  [[nodiscard]] virtual transport::awaitable<transport::expected<size_t>> read(
      std::span<char> data) override;

  [[nodiscard]] virtual transport::awaitable<transport::expected<size_t>> write(
      std::span<const char> data) override;

  [[nodiscard]] virtual std::string name() const override;
  [[nodiscard]] virtual bool message_oriented() const override { return true; }

  [[nodiscard]] virtual bool connected() const override {
    return transport_.connected();
  }

  [[nodiscard]] virtual bool active() const override {
    return transport_.active();
  }

  [[nodiscard]] virtual transport::executor get_executor() override {
    return transport_.get_executor();
  }

 private:
  transport::any_transport transport_;

  std::string incoming_message_;

  bool reading_ = false;

  std::shared_ptr<bool> cancelation_ = std::make_shared<bool>(false);
};
