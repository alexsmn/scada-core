#pragma once

#include "base/boost_log.h"
#include "base/awaitable.h"
#include "base/nested_logger.h"
#include "base/observer_list.h"
#include "base/promise.h"
#include "scada/authentication.h"
#include "scada/services.h"
#include "scada/view_service.h"

#include <functional>
#include <map>
#include <memory>
#include <transport/any_transport.h>
#include <transport/transport_string.h>
#include <optional>

namespace transport {
class TransportFactory;
}  // namespace transport

namespace protocol {
class CreateSession;
class Request;
class Response;
}  // namespace protocol

class Executor;
class RemoteListener;
class ServerConnection;
class SessionStub;
struct CreateSessionResult;

struct RemoteSessionManagerContext {
  const std::shared_ptr<Executor> executor_;
  scada::services services_;
  const scada::Authenticator authenticator_;
  transport::TransportFactory& transport_factory_;
  const std::vector<transport::TransportString> endpoints_;
};

// Accepts remote sessions and managers them.
class RemoteSessionManager final : private RemoteSessionManagerContext {
 public:
  class Observer {
   public:
    virtual void OnSessionOpened(SessionStub& session) {}
    virtual void OnSessionClosed(SessionStub& session) {}
  };

  explicit RemoteSessionManager(RemoteSessionManagerContext&& context);
  virtual ~RemoteSessionManager();

  // Throws an exception on error.
  promise<> Init();

  void CloseUserSessions(const scada::NodeId& user_id);

  void AddObserver(Observer& observer) { observers_.AddObserver(&observer); }
  void RemoveObserver(Observer& observer) {
    observers_.RemoveObserver(&observer);
  }

 private:
  [[nodiscard]] Awaitable<void> InitAsync();

  [[nodiscard]] Awaitable<CreateSessionResult> CreateSessionAsync(
      protocol::CreateSession create_session);

  void DeleteSession(const scada::NodeId& user_node_id);

  bool CheckExistingSession(const scada::NodeId& user_id,
                            const scada::LocalizedText& user_name,
                            bool delete_existing);
  SessionStub& CreateNewSession(const scada::NodeId& user_id,
                                const scada::LocalizedText& user_name);

  SessionStub* FindUserSession(const scada::NodeId& user_id) const;

  void OnSessionAccepted(transport::any_transport transport);
  void OnConnectionClosed(ServerConnection& connection);

  void OnTransportClosed(transport::error_code error);

  const std::shared_ptr<BoostLogger> logger_ =
      std::make_shared<BoostLogger>(LOG_NAME("SessionManager"));

  std::vector<std::shared_ptr<RemoteListener>> listeners_;
  std::vector<std::shared_ptr<ServerConnection>> connections_;

  using SessionMap = std::map<scada::NodeId, std::shared_ptr<SessionStub>>;
  SessionMap session_map_;

  base::ObserverList<Observer> observers_;

  friend class SessionStub;
};
