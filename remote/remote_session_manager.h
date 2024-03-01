#pragma once

#include "base/boost_log.h"
#include "base/nested_logger.h"
#include "base/observer_list.h"
#include "base/promise.h"
#include "scada/authentication.h"
#include "scada/services.h"
#include "scada/view_service.h"

#include <functional>
#include <map>
#include <memory>
#include <net/transport.h>
#include <net/transport_string.h>
#include <optional>

namespace net {
class Logger;
class TransportFactory;
}  // namespace net

namespace protocol {
class CreateSession;
class Request;
class Response;
}  // namespace protocol

class Executor;
class RemoteListener;
class SessionStub;
struct CreateSessionResult;

struct RemoteSessionManagerContext {
  const std::shared_ptr<Executor> executor_;
  scada::services services_;
  const scada::Authenticator authenticator_;
  net::TransportFactory& transport_factory_;
  const std::vector<net::TransportString> endpoints_;
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
  promise<CreateSessionResult> CreateSession(
      const protocol::CreateSession& create_session);
  void DeleteSession(const scada::NodeId& user_node_id);

  bool CheckExistingSession(const scada::NodeId& user_id,
                            const scada::LocalizedText& user_name,
                            bool delete_existing);
  SessionStub& CreateNewSession(const scada::NodeId& user_id,
                                const scada::LocalizedText& user_name);

  SessionStub* FindUserSession(const scada::NodeId& user_id) const;

  void OnSessionAccepted(std::unique_ptr<net::Transport> transport);

  void OnTransportClosed(net::Error error);

  const std::shared_ptr<BoostLogger> logger_ =
      std::make_shared<BoostLogger>(LOG_NAME("SessionManager"));

  std::vector<std::unique_ptr<RemoteListener>> listeners_;

  using SessionMap = std::map<scada::NodeId, std::shared_ptr<SessionStub>>;
  SessionMap session_map_;

  base::ObserverList<Observer> observers_;

  friend class SessionStub;
};
