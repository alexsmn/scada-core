#pragma once

#include <memory>
#include <queue>
#include <string>

#include "net/transport.h"

class ProtocolMessageTransport final : public net::Transport {
 public:
  explicit ProtocolMessageTransport(std::unique_ptr<net::Transport> transport);
  ~ProtocolMessageTransport();

  [[nodiscard]] virtual net::awaitable<net::Error> Open(
      Handlers handlers) override;

  virtual void Close() override;
  [[nodiscard]] virtual int Read(std::span<char> data) override;

  [[nodiscard]] virtual net::awaitable<net::ErrorOr<size_t>> Write(
      std::vector<char> data) override;

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
  void OnTransportOpened();
  void OnTransportClosed(net::Error error);
  void OnTransportDataReceived();

  std::unique_ptr<net::Transport> transport_;

  Handlers handlers_;

  std::string incoming_message_;

  std::shared_ptr<bool> cancelation_ = std::make_shared<bool>(false);
};
