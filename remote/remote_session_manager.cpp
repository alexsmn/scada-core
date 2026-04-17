#include "remote/remote_session_manager.h"

#include "base/awaitable_promise.h"
#include "base/boost_log_adapter.h"
#include "base/debug_util.h"
#include "base/promise_executor.h"
#include "base/utf_convert.h"
#include "model/node_id_util.h"
#include "model/scada_node_ids.h"
#include "net/net_boost_logger_adapter.h"
#include "net/net_executor_adapter.h"
#include "remote/protocol.h"
#include "remote/protocol_message_reader.h"
#include "remote/protocol_message_transport.h"
#include "remote/protocol_utils.h"
#include "remote/remote_connection.h"
#include "remote/remote_listener.h"
#include "remote/session_stub.h"
#include "scada/service_context.h"
#include "scada/status_promise.h"

#include <transport/transport_factory.h>
#include <algorithm>
#include <transport/transport_string.h>
#include <ranges>
#include <utility>

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
  for (auto& connection : connections_) {
    connection->Shutdown();
  }
  connections_.clear();
  listeners_.clear();

  if (!session_map_.empty()) {
    LOG_INFO(*logger_) << "Closing opened sessions"
                       << LOG_TAG("SessionCount", session_map_.size());
    for (const auto& session_stub : session_map_ | std::views::values) {
      session_stub->OnSessionDeleted();
    }
    session_map_.clear();
  }
}

promise<> RemoteSessionManager::Init() {
  return ToPromise(NetExecutorAdapter{executor_}, InitAsync());
}

Awaitable<void> RemoteSessionManager::InitAsync() {
  transport::log_source transport_logger{
      std::make_shared<NetBoostLoggerAdapter>(logger_)};

  // TODO: Captures |this|.
  RemoteListener::AcceptHandler accept_handler =
      [this](transport::any_transport transport) {
        OnSessionAccepted(std::move(transport));
      };

  for (const transport::TransportString& endpoint : endpoints_) {
    auto acceptor = transport_factory_.CreateTransport(
        endpoint, NetExecutorAdapter{executor_}, transport_logger);

    if (!acceptor.ok()) {
      LOG_ERROR(*logger_) << "Cannot create listener transport"
                          << LOG_TAG("Error",
                                     transport::ErrorToShortString(
                                         acceptor.error()));
      throw std::runtime_error{transport::ErrorToShortString(acceptor.error())};
    }

    auto listener_name = acceptor->name();

    auto listener = RemoteListener::Create(logger_, std::move(*acceptor),
                                           std::move(listener_name),
                                           accept_handler);
    listeners_.emplace_back(listener);

    co_await AwaitPromise(NetExecutorAdapter{executor_}, listener->Init());
  }
}

Awaitable<CreateSessionResult> RemoteSessionManager::CreateSessionAsync(
    protocol::CreateSession create_session) {
  auto user_name = scada::ToLocalizedText(
      UtfConvert<char16_t>(create_session.user_name_utf8()));
  auto password = scada::ToLocalizedText(
      UtfConvert<char16_t>(create_session.password_utf8()));
  bool delete_existing = create_session.delete_existing();

  if (!IsCompatibleProtocol(create_session.protocol_version_major(),
                            create_session.protocol_version_minor())) {
    LOG_WARNING(*logger_) << "Unsupported protocol version"
                          << LOG_TAG("UserName", ToString(user_name))
                          << LOG_TAG("VersionMajor",
                                     create_session.protocol_version_major())
                          << LOG_TAG("VersionMinor",
                                     create_session.protocol_version_minor());

    co_return CreateSessionResult{
        .status = scada::StatusCode::Bad_UnsupportedProtocolVersion,
        .protocol_version_major = protocol::PROTOCOL_VERSION_MAJOR,
        .protocol_version_minor = protocol::PROTOCOL_VERSION_MINOR};
  }

  try {
    auto auth_result =
        co_await AwaitPromise(NetExecutorAdapter{executor_},
                              authenticator_(user_name, password));
    auto& user_id = auth_result.user_id;

    LOG_INFO(*logger_) << "Authorization succeeded"
                       << LOG_TAG("UserId", NodeIdToScadaString(user_id))
                       << LOG_TAG("UserName", ToString(user_name))
                       << LOG_TAG("AuthorizationResult",
                                  ToString(auth_result));

    if (!auth_result.multi_sessions &&
        !CheckExistingSession(user_id, user_name, delete_existing)) {
      LOG_WARNING(*logger_) << "Session is already opened"
                            << LOG_TAG("UserId", NodeIdToScadaString(user_id))
                            << LOG_TAG("UserName", ToString(user_name));
      throw scada::status_exception{scada::StatusCode::Bad_UserIsAlreadyLoggedOn};
    }

    auto& session = CreateNewSession(user_id, user_name);

    LOG_INFO(*logger_) << "CreateSessionAsync returning success"
                       << LOG_TAG("UserId", NodeIdToScadaString(user_id));

    co_return CreateSessionResult{
        .status = scada::StatusCode::Good,
        .protocol_version_major = protocol::PROTOCOL_VERSION_MAJOR,
        .protocol_version_minor = protocol::PROTOCOL_VERSION_MINOR,
        .user_id = user_id,
        .user_rights = auth_result.user_rights,
        .session = &session};
  } catch (...) {
    auto status = scada::GetExceptionStatus(std::current_exception());

    LOG_WARNING(*logger_)
        << "Authorization error" << LOG_TAG("UserName", ToString(user_name))
        << LOG_TAG("ErrorString", ToString(status));

    co_return CreateSessionResult{
        .status = status,
        .protocol_version_major = protocol::PROTOCOL_VERSION_MAJOR,
        .protocol_version_minor = protocol::PROTOCOL_VERSION_MINOR};
  }
}

bool RemoteSessionManager::CheckExistingSession(
    const scada::NodeId& user_id,
    const scada::LocalizedText& user_name,
    bool delete_existing) {
  // Disconnect existing session of the same user_node.
  if (SessionStub* existing_session = FindUserSession(user_id)) {
    if (!delete_existing)
      return false;

    LOG_WARNING(*logger_) << "Forced log off existing session"
                          << LOG_TAG("UserId", NodeIdToScadaString(user_id))
                          << LOG_TAG("UserName", user_name)
                          << LOG_TAG(
                                 "Context",
                                 ToString(existing_session->service_context()));
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

  auto session = SessionStub::Create(SessionContext{
      .executor_ = executor_,
      .services_ = services_,
      .service_context_ = scada::ServiceContext{}.with_user_id(user_id)});

  auto& session_ref = *session;
  session_map_.insert_or_assign(user_id, std::move(session));

  LOG_INFO(*logger_) << "Session stored"
                     << LOG_TAG("UserId", NodeIdToScadaString(user_id))
                     << LOG_TAG("SessionCount", session_map_.size());

  // Notify all observers.
  for (auto& o : observers_) {
    LOG_INFO(*logger_) << "Notify session-open observer"
                       << LOG_TAG("UserId", NodeIdToScadaString(user_id));
    o.OnSessionOpened(session_ref);
  }

  LOG_INFO(*logger_) << "Session creation finalized"
                     << LOG_TAG("UserId", NodeIdToScadaString(user_id));

  return session_ref;
}

void RemoteSessionManager::DeleteSession(const scada::NodeId& user_id) {
  // Remove session from map.
  auto i = session_map_.find(user_id);
  assert(i != session_map_.end());
  auto session = std::move(i->second);
  session_map_.erase(i);

  LOG_INFO(*logger_) << "Session deleted"
                     << " | Context = " << session->service_context();

  // Notify all observers.
  for (auto& o : observers_)
    o.OnSessionClosed(*session);
}

void RemoteSessionManager::CloseUserSessions(const scada::NodeId& user_id) {
  Dispatch(*executor_, [this, user_id] {
    if (SessionStub* session = FindUserSession(user_id)) {
      LOG_WARNING(*logger_)
          << "Close session because of user deletion"
          << " | Context = " << session->service_context();
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
    transport::any_transport transport) {
  if (!transport.message_oriented()) {
    transport = transport::any_transport{
        std::make_unique<ProtocolMessageTransport>(std::move(transport))};
  }

  ServerConnectionContext connection_context;
  connection_context.transport_ = std::move(transport);
  connection_context.create_session_handler_ =
      [this](protocol::CreateSession create_session)
          -> Awaitable<CreateSessionResult> {
        co_return co_await CreateSessionAsync(std::move(create_session));
      };
  connection_context.delete_session_handler_ = [this](SessionStub& session) {
    DeleteSession(session.service_context().user_id());
  };
  connection_context.closed_handler_ = [this](ServerConnection& connection) {
    OnConnectionClosed(connection);
  };

  connections_.emplace_back(ServerConnection::Create(std::move(connection_context)));
}

void RemoteSessionManager::OnConnectionClosed(ServerConnection& connection) {
  std::erase_if(connections_, [&connection](const auto& item) {
    return item.get() == &connection;
  });
}

void RemoteSessionManager::OnTransportClosed(transport::error_code error) {
  LOG_WARNING(*logger_) << "Session transport closed"
                        << LOG_TAG("ErrorString",
                                   transport::ErrorToString(error));

  // Don't close session intentionally.
}
