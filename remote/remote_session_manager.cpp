#include "remote/remote_session_manager.h"

#include "base/boost_log_adapter.h"
#include "base/debug_util.h"
#include "base/net_boost_logger_adapter.h"
#include "base/net_executor_adapter.h"
#include "base/promise_executor.h"
#include "base/strings/utf_string_conversions.h"
#include "model/node_id_util.h"
#include "model/scada_node_ids.h"
#include "remote/protocol.h"
#include "remote/protocol_message_reader.h"
#include "remote/protocol_message_transport.h"
#include "remote/protocol_utils.h"
#include "remote/remote_connection.h"
#include "remote/remote_listener.h"
#include "remote/session_stub.h"
#include "scada/service_context.h"
#include "scada/status_promise.h"

#include <net/transport_factory.h>
#include <net/transport_string.h>

#include "base/debug_util-inl.h"

namespace {

inline bool IsCompatibleProtocol(int major, int minor) {
  return major == protocol::PROTOCOL_VERSION_MAJOR &&
         minor <= protocol::PROTOCOL_VERSION_MINOR;
}

}  // namespace

// RemoteSessionManager

RemoteSessionManager::RemoteSessionManager(
    RemoteSessionManagerContext&& context)
    : RemoteSessionManagerContext{std::move(context)} {
  LOG_INFO(*logger_) << "Initialization";
}

RemoteSessionManager::~RemoteSessionManager() {
  if (!session_map_.empty()) {
    LOG_INFO(*logger_) << "Closing opened sessions"
                       << LOG_TAG("SessionCount", session_map_.size());
    for (auto& p : session_map_)
      p.second->OnSessionDeleted();
    session_map_.clear();
  }

  // All connections will be destroyed on |listening_transport_| destructor.
  listeners_.clear();
}

promise<> RemoteSessionManager::Init() {
  auto transport_logger = std::make_shared<NetBoostLoggerAdapter>(logger_);

  // TODO: Captures |this|.
  RemoteListener::SessionAcceptedHandler session_accept_handler =
      [this](std::unique_ptr<net::Transport> transport) {
        OnSessionAccepted(std::move(transport));
      };

  std::vector<promise<>> promises;
  for (auto& endpoint : endpoints_) {
    auto transport = transport_factory_.CreateTransport(
        endpoint, NetExecutorAdapter{executor_}, transport_logger);
    if (!transport) {
      LOG_ERROR(*logger_) << "Cannot create listener transport";
      return make_rejected_promise(std::exception{});
    }

    auto listener_name = transport->GetName();

    auto& listener = listeners_.emplace_back(std::make_unique<RemoteListener>(
        logger_, net::acceptor{std::move(transport)}, std::move(listener_name),
        session_accept_handler));

    promises.emplace_back(listener->Init());
  }

  return make_all_promise_void(std::move(promises));
}

promise<CreateSessionResult> RemoteSessionManager::CreateSession(
    const protocol::CreateSession& create_session) {
  auto user_name = scada::ToLocalizedText(
      base::UTF8ToUTF16(create_session.user_name_utf8()));
  auto password =
      scada::ToLocalizedText(base::UTF8ToUTF16(create_session.password_utf8()));
  bool delete_existing = create_session.delete_existing();

  if (!IsCompatibleProtocol(create_session.protocol_version_major(),
                            create_session.protocol_version_minor())) {
    LOG_WARNING(*logger_) << "Unsupported protocol version"
                          << LOG_TAG("UserName", ToString(user_name))
                          << LOG_TAG("VersionMajor",
                                     create_session.protocol_version_major())
                          << LOG_TAG("VersionMinor",
                                     create_session.protocol_version_minor());
    return make_resolved_promise<CreateSessionResult>(
        {.status = scada::StatusCode::Bad_UnsupportedProtocolVersion,
         .protocol_version_major = protocol::PROTOCOL_VERSION_MAJOR,
         .protocol_version_minor = protocol::PROTOCOL_VERSION_MINOR});
  }

  return authenticator_(user_name, password)
      .then(BindPromiseExecutor(
          executor_,
          [this, user_name,
           delete_existing](const scada::AuthenticationResult& auth_result) {
            auto& user_id = auth_result.user_id;

            LOG_INFO(*logger_)
                << "Authorization succeeded"
                << LOG_TAG("UserId", NodeIdToScadaString(user_id))
                << LOG_TAG("UserName", ToString(user_name))
                << LOG_TAG("AuthorizationResult", ToString(auth_result));

            if (!auth_result.multi_sessions &&
                !CheckExistingSession(user_id, user_name, delete_existing)) {
              LOG_WARNING(*logger_)
                  << "Session is already opened"
                  << LOG_TAG("UserId", NodeIdToScadaString(user_id))
                  << LOG_TAG("UserName", ToString(user_name));
              return scada::MakeRejectedStatusPromise<CreateSessionResult>(
                  scada::StatusCode::Bad_UserIsAlreadyLoggedOn);
            }

            auto& session = CreateNewSession(user_id, user_name);

            return make_resolved_promise(CreateSessionResult{
                .status = scada::StatusCode::Good,
                .protocol_version_major = protocol::PROTOCOL_VERSION_MAJOR,
                .protocol_version_minor = protocol::PROTOCOL_VERSION_MINOR,
                .user_id = user_id,
                .user_rights = auth_result.user_rights,
                .session = &session});
          }))
      .except([this, user_name](const std::exception_ptr& e) {
        auto status = scada::GetExceptionStatus(e);

        LOG_WARNING(*logger_)
            << "Authorization error" << LOG_TAG("UserName", ToString(user_name))
            << LOG_TAG("ErrorString", ToString(status));

        return CreateSessionResult{
            .status = status,
            .protocol_version_major = protocol::PROTOCOL_VERSION_MAJOR,
            .protocol_version_minor = protocol::PROTOCOL_VERSION_MINOR};
      });
}

bool RemoteSessionManager::CheckExistingSession(
    const scada::NodeId& user_id,
    const scada::LocalizedText& user_name,
    bool delete_existing) {
  // Disconnect existing session of the same user_node.
  if (auto* existing_session = FindUserSession(user_id)) {
    if (!delete_existing)
      return false;

    LOG_WARNING(*logger_)
        << "Forced log off existing session"
        << LOG_TAG("UserId", NodeIdToScadaString(user_id))
        << LOG_TAG("UserName", user_name)
        << LOG_TAG("Context", ToString(*existing_session->service_context()));
    existing_session->OnSessionDeleted();
    DeleteSession(user_id);
  }

  return true;
}

SessionStub& RemoteSessionManager::CreateNewSession(
    const scada::NodeId& user_id,
    const scada::LocalizedText& user_name) {
  LOG_INFO(*logger_) << "Create session"
                     << LOG_TAG("UserId", NodeIdToScadaString(user_id))
                     << LOG_TAG("UserName", ToString(user_name));

  auto service_context = std::make_shared<const scada::ServiceContext>(
      scada::ServiceContext{.user_id = user_id});

  auto session =
      SessionStub::Create(SessionContext{.executor_ = executor_,
                                         .services_ = services_,
                                         .service_context_ = service_context});

  auto& session_ref = *session;
  session_map_[user_id] = std::move(session);

  // Notify all observers.
  for (auto& o : observers_)
    o.OnSessionOpened(session_ref);

  return session_ref;
}

void RemoteSessionManager::DeleteSession(const scada::NodeId& user_id) {
  // Remove session from map.
  auto i = session_map_.find(user_id);
  assert(i != session_map_.end());
  auto session = std::move(i->second);
  session_map_.erase(i);

  LOG_INFO(*logger_) << "Session deleted"
                     << LOG_TAG("Context",
                                ToString(*session->service_context()));

  // Notify all observers.
  for (auto& o : observers_)
    o.OnSessionClosed(*session);
}

void RemoteSessionManager::CloseUserSessions(const scada::NodeId& user_id) {
  Dispatch(*executor_, [this, user_id] {
    if (SessionStub* session = FindUserSession(user_id)) {
      LOG_WARNING(*logger_)
          << "Close session because of user deletion"
          << LOG_TAG("Context", ToString(*session->service_context()));
      session->OnSessionDeleted();
      DeleteSession(user_id);
    }
  });
}

SessionStub* RemoteSessionManager::FindUserSession(
    const scada::NodeId& user_id) const {
  auto i = session_map_.find(user_id);
  return i != session_map_.end() ? i->second.get() : nullptr;
}

void RemoteSessionManager::OnSessionAccepted(
    std::unique_ptr<net::Transport> transport) {
  auto message_transport = std::move(transport);
  if (!message_transport->IsMessageOriented()) {
    auto protocol_transport = std::make_unique<ProtocolMessageTransport>(
        std::move(message_transport));
    message_transport = std::move(protocol_transport);
  }

  ServerConnectionContext connection_context;
  connection_context.transport_ = std::move(message_transport);
  connection_context.create_session_handler_ =
      [this](const protocol::Request& request,
             const CreateSessionCallback& callback) {
        CreateSession(request.create_session()).then(callback);
      };
  connection_context.delete_session_handler_ = [this](SessionStub& session) {
    DeleteSession(session.service_context()->user_id);
  };

  new ServerConnection(std::move(connection_context));
}

void RemoteSessionManager::OnTransportClosed(net::Error error) {
  LOG_WARNING(*logger_) << "Session transport closed"
                        << LOG_TAG("ErrorString", net::ErrorToString(error));

  // Don't close session intentionally.
}
