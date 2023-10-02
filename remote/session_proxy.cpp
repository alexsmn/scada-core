#include "remote/session_proxy.h"

#include "base/net_boost_logger_adapter.h"
#include "base/string_piece_util.h"
#include "base/strings/string_split.h"
#include "base/strings/sys_string_conversions.h"
#include "base/strings/utf_string_conversions.h"
#include "net/transport_factory.h"
#include "net/transport_string.h"
#include "remote/history_proxy.h"
#include "remote/node_management_proxy.h"
#include "remote/protocol.h"
#include "remote/protocol_message_reader.h"
#include "remote/protocol_message_transport.h"
#include "remote/protocol_utils.h"
#include "remote/session_proxy_debuger.h"
#include "remote/subscription_proxy.h"
#include "remote/view_service_proxy.h"
#include "scada/monitored_item.h"
#include "scada/status.h"

#include <stdexcept>

using namespace std::chrono_literals;

namespace {

const auto kPingDelay = 1s;

std::string MakeConnectionString(std::string_view host_name) {
  auto parts =
      base::SplitString(AsStringPiece(host_name), ":", base::TRIM_WHITESPACE,
                        base::SplitResult::SPLIT_WANT_ALL);
  parts.resize(2);
  if (parts[1].empty())
    parts[1] = "2000";
  return std::format("Host={};Port={}", parts[0], parts[1]);
}

}  // namespace

SessionProxy::SessionProxy(SessionProxyContext&& context)
    : SessionProxyContext{std::move(context)},
      debugger_{std::make_unique<SessionProxyDebugger>()},
      subscription_{std::make_shared<SubscriptionProxy>(SubscriptionParams{})},
      view_service_proxy_{std::make_unique<ViewServiceProxy>()},
      node_management_proxy_{std::make_unique<NodeManagementProxy>()},
      history_proxy_{std::make_unique<HistoryProxy>()},
      ping_timer_{executor_} {}

SessionProxy::~SessionProxy() = default;

scada::services SessionProxy::services() {
  return scada::services{
      .attribute_service = this,
      .monitored_item_service = this,
      .method_service = this,
      .history_service = history_proxy_.get(),
      .view_service = view_service_proxy_.get(),
      .node_management_service = node_management_proxy_.get(),
      .session_service = this};
}

promise<> SessionProxy::Disconnect() {
  if (!session_created_)
    return MakeRejectedStatusPromise(scada::StatusCode::Bad_Disconnected);

  protocol::Request request;
  auto& delete_session = *request.mutable_delete_session();
  delete_session;

  promise<> promise;
  Request(request, [this, promise](const protocol::Response& response) mutable {
    auto status = ConvertTo<scada::Status>(response.status());
    if (status)
      OnSessionDeleted();

    ResolveStatusPromise(promise, std::move(status));
  });
  return promise;
}

void SessionProxy::OnTransportOpened() {
  LOG_INFO(*logger_) << "Transport opened";

  protocol::Request request;
  auto& create_session = *request.mutable_create_session();
  create_session.set_user_name_utf8(base::UTF16ToUTF8(user_name_));
  create_session.set_password_utf8(base::UTF16ToUTF8(password_));
  create_session.set_protocol_version_major(protocol::PROTOCOL_VERSION_MAJOR);
  create_session.set_protocol_version_minor(protocol::PROTOCOL_VERSION_MINOR);
  if (allow_remote_logoff_)
    create_session.set_delete_existing(true);

  Request(request, [this](const protocol::Response& response) {
    OnCreateSessionResult(response);
  });
}

void SessionProxy::OnSessionCreated() {
  assert(!session_created_);
  session_created_ = true;

  subscription_->OnChannelOpened(*this);
  view_service_proxy_->OnChannelOpened(*this);
  node_management_proxy_->OnChannelOpened(*this);
  history_proxy_->OnChannelOpened(*this);

  SchedulePing();

  session_state_changed_signal_(true, scada::StatusCode::Good);

  ForwardConnectResult(scada::StatusCode::Good);
}

void SessionProxy::OnTransportClosed(net::Error error) {
  LOG_WARNING(*logger_) << "Transport closed"
                        << LOG_TAG("Status", ErrorToString(error));

  scada::Status status_code(error == net::OK
                                ? scada::StatusCode::Bad_SessionForcedLogoff
                            : error == net::ERR_SSL_BAD_PEER_PUBLIC_KEY
                                ? scada::StatusCode::Bad_UserIsAlreadyLoggedOn
                                : scada::StatusCode::Bad_Disconnected);
  OnSessionError(status_code);
}

void SessionProxy::OnSessionError(const scada::Status& status) {
  transport_.reset();

  OnSessionDeleted();

  auto copied_status = status;
  ForwardConnectResult(std::move(copied_status));

  session_state_changed_signal_(false, status);
}

void SessionProxy::OnTransportMessageReceived(std::span<const char> data) {
  protocol::Message message;
  if (!message.ParseFromArray(data.data(), data.size()))
    throw std::runtime_error("Can't parse message");

  OnMessageReceived(message);
}

void SessionProxy::OnSessionDeleted() {
  // When OnSessionCloased is called on connection, channel actually is still
  // not opened.
  if (!session_created_)
    return;

  session_created_ = false;

  // Cancel all requests.
  {
    protocol::Response response;
    Convert(scada::Status{scada::StatusCode::Bad_Disconnected},
            *response.mutable_status());
    auto requests = std::move(requests_);
    for (auto& p : requests) {
      response.set_request_id(p.first);
      if (p.second) {
        p.second(response);
      }
      debugger_->NotifyRequestEvent(
          {.id = static_cast<scada::SessionDebugger::RequestId>(p.first),
           .phase = scada::SessionDebugger::RequestPhase::Failed});
    }
  }

  ping_time_ = {};
  ping_timer_.Stop();

  subscription_->OnChannelClosed();
  node_management_proxy_->OnChannelClosed();
  view_service_proxy_->OnChannelClosed();
  history_proxy_->OnChannelClosed();
}

bool SessionProxy::HasPrivilege(scada::Privilege privilege) const {
  return (user_rights_ & (1 << static_cast<int>(privilege))) != 0;
}

void SessionProxy::Send(protocol::Message& message) {
  assert(message.IsInitialized());

  // TODO: This check shall be changed on assert when all proxy object will
  // support MonitoredItemService reconnection.
  if (!transport_.get()) {
    assert(false);
    return;
  }

  if (IsMessageLogged(message)) {
    LOG_INFO(*logger_) << "Send message"
                       << LOG_TAG("Message", message.DebugString());
  }

  std::string string;
  if (!message.AppendToString(&string))
    throw std::runtime_error("Can't serialize message");

  // TODO: Handle write result.
  transport_->Write(string);
}

void SessionProxy::OnMessageReceived(const protocol::Message& message) {
  if (IsMessageLogged(message)) {
    LOG_INFO(*logger_) << "Message received"
                       << LOG_TAG("Message", message.DebugString());
  }

  for (auto& response : message.responses()) {
    auto i = requests_.find(response.request_id());
    if (i != requests_.end()) {
      auto handler = std::move(i->second);
      requests_.erase(i);
      debugger_->NotifyRequestEvent(
          {.id = static_cast<scada::SessionDebugger::RequestId>(
               response.request_id()),
           .phase = scada::SessionDebugger::RequestPhase::Succeeded,
           .title = response.GetTypeName(),
           .response_body = response.DebugString()});
      handler(response);
    }
  }

  for (auto& notification : message.notifications()) {
    debugger_->NotifyRequestEvent(
        {.phase = scada::SessionDebugger::RequestPhase::Succeeded,
         .title = notification.GetTypeName(),
         .body = notification.DebugString()});

    auto status_code =
        notification.has_status_code()
            ? ConvertTo<scada::StatusCode>(notification.status_code())
            : scada::StatusCode::Good;

    for (auto& data_change : notification.data_changes()) {
      subscription_->OnDataChange(
          data_change.monitored_item_id(),
          ConvertTo<scada::DataValue>(data_change.data_value()));
    }

    for (auto& model_change : notification.model_change()) {
      subscription_->OnEvent(notification.monitored_item_id(), status_code,
                             ConvertTo<scada::ModelChangeEvent>(model_change));
    }

    for (auto& semantics_changed_node_id :
         notification.semantics_changed_node_id()) {
      subscription_->OnEvent(
          notification.monitored_item_id(), status_code,
          scada::SemanticChangeEvent{
              ConvertTo<scada::NodeId>(semantics_changed_node_id)});
    }

    for (auto& event : notification.events()) {
      subscription_->OnEvent(notification.monitored_item_id(), status_code,
                             ConvertTo<scada::Event>(event));
    }

    if (notification.has_session_deleted()) {
      OnSessionError(scada::StatusCode::Bad_ServerWasShutDown);
      return;
    }
  }
}

void SessionProxy::Request(protocol::Request& request,
                           ResponseHandler response_handler) {
  auto request_id = next_request_id_;
  next_request_id_ = next_request_id_ == std::numeric_limits<int>::max()
                         ? 1
                         : (next_request_id_ + 1);

  if (response_handler)
    requests_[request_id] = std::move(response_handler);

  debugger_->NotifyRequestEvent(
      {.id = request_id,
       .phase = scada::SessionDebugger::RequestPhase::Running,
       .title = request.GetTypeName(),
       .body = request.DebugString()});

  request.set_request_id(request_id);
  assert(request.IsInitialized());

  protocol::Message message;
  message.add_requests()->Swap(&request);
  Send(message);
}

promise<> SessionProxy::Connect(const scada::SessionConnectParams& params) {
  assert(!transport_);

  if (session_created_)
    return MakeRejectedStatusPromise(scada::StatusCode::Bad);

  user_name_ = params.user_name;
  password_ = params.password;
  host_ = params.host;
  // TODO: Add a UT for this condition.
  connection_string_ = host_.empty() ? params.connection_string
                                     : MakeConnectionString(params.host);
  allow_remote_logoff_ = params.allow_remote_logoff;

  return Reconnect();
}

promise<> SessionProxy::Connect() {
  connect_promise_ = promise<>{};

  LOG_INFO(*logger_) << "Connect" << LOG_TAG("UserName", user_name_)
                     << LOG_TAG("ConnectionString", connection_string_);

  auto transport_logger = std::make_shared<NetBoostLoggerAdapter>(logger_);
  auto transport = transport_factory_.CreateTransport(
      net::TransportString{connection_string_}, std::move(transport_logger));
  if (!transport) {
    LOG_WARNING(*logger_) << "Cannot create raw transport";
    OnTransportClosed(net::ERR_FAILED);
    return connect_promise_;
  }

  transport_ = std::make_unique<ProtocolMessageTransport>(std::move(transport));

  transport_->Open(
      {.on_open = [this] { OnTransportOpened(); },
       .on_close = [this](net::Error error) { OnTransportClosed(error); },
       .on_message =
           [this](std::span<const char> data) {
             OnTransportMessageReceived(data);
           }});

  return connect_promise_;
}

void SessionProxy::ForwardConnectResult(scada::Status&& status) {
  ResolveStatusPromise(connect_promise_, std::move(status));
}

void SessionProxy::OnCreateSessionResult(const protocol::Response& response) {
  auto status = ConvertTo<scada::Status>(response.status());
  if (!status) {
    OnSessionError(status);
    return;
  }

  if (!response.has_create_session_result()) {
    OnSessionError(scada::StatusCode::Bad);
    return;
  }

  auto& create_session_result = response.create_session_result();
  session_token_ = create_session_result.token();
  Convert(create_session_result.user_node_id(), user_node_id_);
  user_rights_ = create_session_result.user_rights();

  OnSessionCreated();
}

void SessionProxy::Read(
    const std::shared_ptr<const scada::ServiceContext>& context,
    const std::shared_ptr<const std::vector<scada::ReadValueId>>& inputs,
    const scada::ReadCallback& callback) {
  if (!session_created_) {
    callback(scada::StatusCode::Bad_Disconnected, {});
    return;
  }

  protocol::Request request;
  auto& read = *request.mutable_read();
  for (auto& value_id : *inputs)
    Convert(value_id, *read.add_value_id());

  Request(request, [callback](const protocol::Response& response) {
    if (callback)
      callback(ConvertTo<scada::Status>(response.status()),
               ConvertTo<std::vector<scada::DataValue>>(
                   response.read_result().value()));
  });
}

void SessionProxy::Write(
    const std::shared_ptr<const scada::ServiceContext>& context,
    const std::shared_ptr<const std::vector<scada::WriteValue>>& inputs,
    const scada::WriteCallback& callback) {
  if (!session_created_) {
    callback(scada::StatusCode::Bad_Disconnected, {});
    return;
  }

  protocol::Request request;
  for (auto& value : *inputs)
    Convert(value, *request.add_write());

  Request(request, [callback](const protocol::Response& response) {
    if (callback) {
      callback(
          ConvertTo<scada::Status>(response.status()),
          ConvertTo<std::vector<scada::StatusCode>>(response.write_result()));
    }
  });
}

void SessionProxy::Call(const scada::NodeId& node_id,
                        const scada::NodeId& method_id,
                        const std::vector<scada::Variant>& arguments,
                        const scada::NodeId& user_id,
                        const scada::StatusCallback& callback) {
  if (!session_created_) {
    callback(scada::StatusCode::Bad_Disconnected);
    return;
  }

  protocol::Request request;
  auto& command = *request.mutable_call()->mutable_device_command();
  Convert(node_id, *command.mutable_node_id());
  Convert(method_id, *command.mutable_method_id());
  Convert(arguments, *command.mutable_argument());

  Request(request, [callback](const protocol::Response& response) {
    if (callback)
      callback(ConvertTo<scada::Status>(response.status()));
  });
}

std::shared_ptr<scada::MonitoredItem> SessionProxy::CreateMonitoredItem(
    const scada::ReadValueId& read_value_id,
    const scada::MonitoringParameters& params) {
  return subscription_->CreateMonitoredItem(read_value_id, params);
}

promise<> SessionProxy::Reconnect() {
  if (!session_created_) {
    return Connect();
  }

  return Disconnect().then([this] { return Connect(); });
}

bool SessionProxy::IsConnected(base::TimeDelta* ping_delay) const {
  if (ping_delay) {
    *ping_delay = last_ping_delay_;
    if (!ping_time_.is_null())
      *ping_delay = std::max(*ping_delay, base::TimeTicks::Now() - ping_time_);
  }
  return session_created_;
}

scada::NodeId SessionProxy::GetUserId() const {
  return user_node_id_;
}

std::string SessionProxy::GetHostName() const {
  return host_;
}

void SessionProxy::SchedulePing() {
  ping_timer_.StartOne(kPingDelay, [this] { Ping(); });
}

void SessionProxy::Ping() {
  assert(ping_time_.is_null());

  ping_time_ = base::TimeTicks::Now();

  protocol::Request request;
  auto& ping = *request.mutable_ping();
  ping;

  Request(request, [this](const protocol::Response& response) {
    last_ping_delay_ = base::TimeTicks::Now() - ping_time_;
    ping_time_ = {};
    SchedulePing();
  });
}

bool SessionProxy::IsMessageLogged(const protocol::Message& message) const {
  for (const auto& request : message.requests()) {
    if (service_log_params_.log_read) {
      if (request.has_read())
        return true;
    }
    if (service_log_params_.log_browse) {
      if (request.has_browse())
        return true;
    }
    if (service_log_params_.log_history) {
      if (request.has_history_read_raw() || request.has_history_read_events())
        return true;
    }
  }

  for (const auto& response : message.responses()) {
    if (service_log_params_.log_read) {
      if (response.has_read_result())
        return true;
    }
    if (service_log_params_.log_browse) {
      if (response.has_browse_result())
        return true;
    }
    if (service_log_params_.log_history) {
      if (response.has_history_read_raw_result() ||
          response.has_history_read_events_result())
        return true;
    }
  }

  if (service_log_params_.log_event ||
      service_log_params_.log_model_change_event ||
      service_log_params_.log_node_semantics_change_event) {
    for (const auto& notification : message.notifications()) {
      if (service_log_params_.log_event) {
        if (!notification.events().empty())
          return true;
      }
      if (service_log_params_.log_model_change_event) {
        if (!notification.model_change().empty())
          return true;
      }
      if (service_log_params_.log_node_semantics_change_event) {
        if (!notification.semantics_changed_node_id().empty())
          return true;
      }
    }
  }

  return false;
}

boost::signals2::scoped_connection SessionProxy::SubscribeSessionStateChanged(
    const SessionStateChangedCallback& callback) {
  return session_state_changed_signal_.connect(callback);
}

scada::SessionDebugger* SessionProxy::GetSessionDebugger() {
  return debugger_.get();
}
