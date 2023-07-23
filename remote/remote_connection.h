#pragma once

#include <functional>
#include <memory>

#include "scada/node_id.h"
#include "scada/status.h"
#include "net/transport.h"
#include "remote/connection.h"

namespace protocol {
class Request;
class Response;
}  // namespace protocol

class SessionStub;

struct CreateSessionResult {
  scada::Status status;
  unsigned protocol_version_major = 0;
  unsigned protocol_version_minor = 0;
  scada::NodeId user_id;
  unsigned user_rights = 0;
  SessionStub* session = nullptr;
};

using CreateSessionCallback =
    std::function<void(const CreateSessionResult& result)>;

struct ServerConnectionContext {
  std::unique_ptr<net::Transport> transport_;
  std::function<void(const protocol::Request&, CreateSessionCallback)>
      create_session_handler_;
  std::function<void(SessionStub&)> delete_session_handler_;
};

class ServerConnection : public Connection, private ServerConnectionContext {
 public:
  explicit ServerConnection(ServerConnectionContext&& context);
  virtual ~ServerConnection();

  virtual void Send(protocol::Message& message) override;
  virtual void OnSessionDeleted() override;

 private:
  void Close();

  void OnCreateSession(const protocol::Request& request);
  void OnDeleteSession(const protocol::Request& request);

  void OnTransportClosed(net::Error error);
  void OnTransportMessageReceived(std::span<const char> data);

  SessionStub* session_ = nullptr;

  std::shared_ptr<bool> cancelation_ = std::make_shared<bool>(false);
};
