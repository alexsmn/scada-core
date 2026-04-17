#pragma once

#include "base/boost_log.h"
#include "remote/connection.h"
#include "base/awaitable.h"
#include "base/promise.h"
#include "scada/node_id.h"
#include "scada/status.h"

#include <functional>
#include <memory>
#include <vector>
#include <transport/any_transport.h>
#include <transport/write_queue.h>

namespace protocol {
class CreateSession;
class Request;
class Response;
}  // namespace protocol

class SessionStub;
class ServerConnection;

struct CreateSessionResult {
  scada::Status status = scada::StatusCode::Good;
  unsigned protocol_version_major = 0;
  unsigned protocol_version_minor = 0;
  scada::NodeId user_id;
  unsigned user_rights = 0;
  SessionStub* session = nullptr;
};

struct ServerConnectionContext {
  transport::any_transport transport_;
  std::function<Awaitable<CreateSessionResult>(protocol::CreateSession)>
      create_session_handler_;
  std::function<void(SessionStub&)> delete_session_handler_;
  std::function<void(ServerConnection&)> closed_handler_;
};

class ServerConnection : public Connection,
                         private ServerConnectionContext,
                         public std::enable_shared_from_this<ServerConnection> {
 public:
  static std::shared_ptr<ServerConnection> Create(
      ServerConnectionContext&& context);
  virtual ~ServerConnection();

  [[nodiscard]] transport::awaitable<void> Run();

  virtual void Send(protocol::Message& message) override;
  virtual void OnSessionDeleted() override;
  void Shutdown();

 private:
  explicit ServerConnection(ServerConnectionContext&& context);

  void Start();
  void Close();

  void OnCreateSession(const protocol::Request& request);
  [[nodiscard]] Awaitable<void> OnCreateSessionAsync(protocol::Request request);
  void OnDeleteSession(const protocol::Request& request);
  [[nodiscard]] Awaitable<void> OnDeleteSessionAsync(protocol::Request request);

  void OnTransportClosed(transport::error_code error);
  void OnTransportMessageReceived(std::span<const char> data);

  SessionStub* session_ = nullptr;

  const std::shared_ptr<BoostLogger> logger_ =
      std::make_shared<BoostLogger>(LOG_NAME("ServerConnection"));

  transport::WriteQueue write_queue_{transport_};

  std::shared_ptr<bool> cancelation_ = std::make_shared<bool>(false);
  bool closed_ = false;
};
