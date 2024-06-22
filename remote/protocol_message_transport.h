#pragma once

#include <memory>
#include <queue>
#include <string>

#include "net/transport.h"

// Operates with unframed messages. Automatically adds frames on send and
// removes on reception.
class ProtocolMessageTransport final : public net::Transport {
 public:
  explicit ProtocolMessageTransport(std::unique_ptr<net::Transport> transport);
  ~ProtocolMessageTransport();

  [[nodiscard]] virtual net::awaitable<net::Error> Open() override;

  virtual void Close() override;

  [[nodiscard]] virtual net::awaitable<
      net::ErrorOr<std::unique_ptr<net::Transport>>>
  Accept() override;

  [[nodiscard]] virtual net::awaitable<net::ErrorOr<size_t>> Read(
      std::span<char> data) override;

  [[nodiscard]] virtual net::awaitable<net::ErrorOr<size_t>> Write(
      std::span<const char> data) override;

  [[nodiscard]] virtual std::string GetName() const override;
  [[nodiscard]] virtual bool IsMessageOriented() const override { return true; }

  [[nodiscard]] virtual bool IsConnected() const override {
    return transport_->IsConnected();
  }

  [[nodiscard]] virtual bool IsActive() const override {
    return transport_->IsActive();
  }

  [[nodiscard]] virtual net::Executor GetExecutor() const override {
    return transport_->GetExecutor();
  }

 private:
  std::unique_ptr<net::Transport> transport_;

  std::string incoming_message_;

  bool reading_ = false;

  std::shared_ptr<bool> cancelation_ = std::make_shared<bool>(false);
};
