#pragma once

#include <memory>
#include <queue>
#include <string>

#include "net/transport.h"

class ProtocolMessageTransport final : public net::Transport {
 public:
  explicit ProtocolMessageTransport(std::unique_ptr<net::Transport> transport);
  ~ProtocolMessageTransport();

  virtual net::promise<void> Open(const Handlers& handlers) override;
  virtual void Close() override;
  virtual int Read(std::span<char> data) override;
  virtual net::promise<size_t> Write(std::span<const char> data) override;
  virtual std::string GetName() const override;
  virtual bool IsMessageOriented() const override { return true; }
  virtual bool IsConnected() const override {
    return transport_->IsConnected();
  }
  virtual bool IsActive() const override { return transport_->IsActive(); }

 private:
  void OnTransportOpened();
  void OnTransportClosed(net::Error error);
  void OnTransportDataReceived();

  std::unique_ptr<net::Transport> transport_;

  Handlers handlers_;

  std::string incoming_message_;

  std::shared_ptr<bool> cancelation_;
};
