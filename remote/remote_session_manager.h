#pragma once

#include "base/any_executor.h"
#include "base/awaitable.h"
#include "base/boost_log.h"
#include "base/lifetime.h"
#include "metrics/tracer.h"
#include "scada/authentication.h"
#include "scada/co_result.h"
#include "scada/services.h"
#include "scada/status_or.h"
#include "scada/view_service.h"

#include <boost/signals2/connection.hpp>
#include <boost/signals2/signal.hpp>
#include <functional>
#include <map>
#include <memory>
#include <optional>
#include <transport/any_transport.h>
#include <transport/transport_string.h>
#include <utility>

namespace transport {
class TransportFactory;
}  // namespace transport

namespace protocol {
class CreateSession;
class Request;
class Response;
}  // namespace protocol

class RemoteListener;
class ServerConnection;
class SessionStub;
struct CreateSessionResult;

struct RemoteSessionManagerContext {
  const AnyExecutor executor_;
  scada::services services_;
  std::shared_ptr<scada::CoroutineAuthenticator> authenticator_;
  transport::TransportFactory& transport_factory_;
  const std::vector<transport::TransportString> endpoints_;
  // Passed to every session stub for inbound request SERVER spans.
  Tracer& tracer_ = Tracer::None();
};

// Remote login/session lifecycle coordinator. Owns listeners, accepted
// connections, and logical sessions for the remote protocol, and keeps the
// create-session path on a coroutine-first authentication/session-management
// flow.
class RemoteSessionManager final : private RemoteSessionManagerContext {
 public:
  using SessionOpenedCallback = std::function<void(SessionStub& session)>;
  using SessionClosedCallback = std::function<void(SessionStub& session)>;

  explicit RemoteSessionManager(RemoteSessionManagerContext&& context);
  virtual ~RemoteSessionManager();

  [[nodiscard]] scada::CoStatus InitAsync();
  [[nodiscard]] Awaitable<void> ShutdownAsync();

  void CloseUserSessions(const scada::NodeId& user_id);

  // Notifies whenever a new logical session has been created and stored.
  [[nodiscard]] boost::signals2::scoped_connection SubscribeSessionOpened(
      const SessionOpenedCallback& callback);
  // Notifies right after a logical session has been removed; the session
  // object is still alive for the duration of the callback.
  [[nodiscard]] boost::signals2::scoped_connection SubscribeSessionClosed(
      const SessionClosedCallback& callback);

 private:
  [[nodiscard]] Awaitable<CreateSessionResult> CreateSessionAsync(
      protocol::CreateSession create_session);

  void DeleteSession(const scada::NodeId& user_node_id);

  bool CheckExistingSession(const scada::NodeId& user_id,
                            const scada::LocalizedText& user_name,
                            bool delete_existing);
  SessionStub& CreateNewSession(const scada::NodeId& user_id,
                                const scada::LocalizedText& user_name,
                                unsigned user_rights) SCADA_LIFETIME_BOUND;

  SessionStub* FindUserSession(const scada::NodeId& user_id) const
      SCADA_LIFETIME_BOUND;

  void OnSessionAccepted(transport::any_transport transport);
  void OnConnectionClosed(ServerConnection& connection);

  void OnTransportClosed(transport::error_code error);

  const std::shared_ptr<BoostLogger> logger_ =
      std::make_shared<BoostLogger>(LOG_NAME("SessionManager"));

  std::vector<std::shared_ptr<RemoteListener>> listeners_;
  std::vector<std::shared_ptr<ServerConnection>> connections_;

  using SessionMap = std::map<scada::NodeId, std::shared_ptr<SessionStub>>;
  SessionMap session_map_;

  boost::signals2::signal<void(SessionStub&)> session_opened_signal_;
  boost::signals2::signal<void(SessionStub&)> session_closed_signal_;
  std::shared_ptr<bool> alive_ = std::make_shared<bool>(true);

  friend class SessionStub;
};
