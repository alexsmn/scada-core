#include "remote/session_stub.h"

#include "base/awaitable.h"
#include "base/executor.h"
#include "base/range_util.h"
#include "model/node_id_util.h"
#include "remote/connection.h"
#include "remote/history_stub.h"
#include "remote/node_management_stub.h"
#include "remote/protocol.h"
#include "remote/protocol_utils.h"
#include "remote/subscription_stub.h"
#include "remote/view_service_stub.h"
#include "scada/attribute_service.h"
#include "scada/method_service.h"
#include "scada/monitoring_parameters.h"
#include "scada/service_context.h"
#include "scada/service_awaitable.h"
#include "scada/write_flags.h"
#include <boost/range/adaptor/transformed.hpp>

SessionStub::SessionStub(SessionContext&& context)
    : SessionContext(std::move(context)) {
  LOG_BIND_TAG(logger_, "UserId",
               NodeIdToScadaString(service_context_.user_id()));
  LOG_INFO(logger_) << "Created";
}

SessionStub::~SessionStub() {
  LOG_INFO(logger_) << "Destroying";

  if (connection_)
    connection_->OnSessionDeleted();
}

void SessionStub::Init() {
  const std::weak_ptr<MessageSender> sender = weak_from_this();

  if (services_.view_service) {
    view_service_stub_ =
        std::make_shared<ViewServiceStub>(ViewServiceStubContext{
            executor_, sender, service_context_, *services_.view_service});
  }

  if (services_.node_management_service) {
    node_management_stub_ = std::make_shared<NodeManagementStub>(
        executor_, sender, *services_.node_management_service,
        service_context_);
  }

  if (services_.history_service) {
    history_stub_ = std::make_shared<HistoryStub>(*services_.history_service,
                                                  sender, executor_);
  }
}

std::shared_ptr<SessionStub> SessionStub::Create(SessionContext&& context) {
  auto session =
      std::shared_ptr<SessionStub>(new SessionStub(std::move(context)));
  session->Init();
  return session;
}

void SessionStub::SetConnection(Connection* connection) {
  connection_ = connection;

  if (connection_ && send_message_) {
    auto message = std::move(send_message_);
    connection_->Send(*message);
  }
}

void SessionStub::ProcessMessage(const protocol::Message& message) {
  for (auto& request : message.requests())
    ProcessRequest(request);
}

void SessionStub::ProcessRequest(const protocol::Request& request) {
  LOG_INFO(logger_) << "Process request"
                    << LOG_TAG("RequestId", request.request_id())
                    << LOG_TAG("HasPing", request.has_ping())
                    << LOG_TAG("HasRead", request.has_read())
                    << LOG_TAG("WriteCount", request.write_size())
                    << LOG_TAG("HasCall", request.has_call())
                    << LOG_TAG("HasCreateSubscription",
                               request.has_create_subscription())
                    << LOG_TAG("HasDeleteSubscription",
                               request.has_delete_subscription())
                    << LOG_TAG("HasCreateMonitoredItem",
                               request.has_create_monitored_item())
                    << LOG_TAG("HasDeleteMonitoredItem",
                               request.has_delete_monitored_item())
                    << LOG_TAG("HasBrowse", request.has_browse())
                    << LOG_TAG("BrowsePathCount", request.browse_path_size());

  if (request.has_ping()) {
    protocol::Response response;
    response.set_request_id(request.request_id());
    Convert(scada::Status{scada::StatusCode::Good}, *response.mutable_status());
    SendResponse(response);
  }

  OnRead(request);

  OnWrite(request);

  if (request.has_call()) {
    auto& call = request.call();

    if (call.has_device_command()) {
      auto& device_command = call.device_command();
      OnCall(request.request_id(),
             ConvertTo<scada::NodeId>(device_command.node_id()),
             ConvertTo<scada::NodeId>(device_command.method_id()),
             ConvertTo<std::vector<scada::Variant>>(device_command.argument()));
    }
  }

  if (request.has_create_subscription()) {
    auto& create_subscription = request.create_subscription();
    create_subscription;
    OnCreateSubscription(request.request_id());
  }

  if (request.has_delete_subscription()) {
    auto& delete_subscription = request.delete_subscription();
    OnDeleteSubscription(request.has_create_subscription(),
                         delete_subscription.subscription_id());
  }

  if (request.has_create_monitored_item()) {
    auto& create_monitored_item = request.create_monitored_item();

    auto read_value_id = scada::ReadValueId{
        ConvertTo<scada::NodeId>(create_monitored_item.node_id()),
        static_cast<scada::AttributeId>(create_monitored_item.attribute_id())};

    auto monitoring_params =
        create_monitored_item.has_monitoring_parameters()
            ? ConvertTo<scada::MonitoringParameters>(
                  create_monitored_item.monitoring_parameters())
            : scada::MonitoringParameters{};

    OnCreateMonitoredItem(
        request.request_id(), create_monitored_item.subscription_id(),
        std::move(read_value_id), std::move(monitoring_params));
  }

  if (request.has_delete_monitored_item()) {
    auto& delete_monitored_item = request.delete_monitored_item();
    OnDeleteMonitoredItem(request.request_id(),
                          delete_monitored_item.subscription_id(),
                          delete_monitored_item.monitored_item_id());
  }

  if (view_service_stub_)
    view_service_stub_->OnRequestReceived(request);
  if (node_management_stub_)
    node_management_stub_->OnRequestReceived(request);
  if (history_stub_)
    history_stub_->OnRequestReceived(request);
}

void SessionStub::Send(protocol::Message& message) {
  if (connection_) {
    connection_->Send(message);
  } else {
    if (send_message_) {
      send_message_->MergeFrom(message);
    } else {
      send_message_.reset(new protocol::Message);
      send_message_->Swap(&message);
    }
  }
}

void SessionStub::OnCreateSubscription(int request_id) {
  SubscriptionParams params;

  auto subscription_id = next_subscription_id_++;

  std::weak_ptr<MessageSender> sender = weak_from_this();

  auto subscription = std::make_shared<SubscriptionStub>(
      executor_, sender, *services_.monitored_item_service, subscription_id,
      params);
  subscriptions_.emplace(subscription_id, std::move(subscription));

  protocol::Message message;
  auto& response = *message.add_responses();
  response.set_request_id(request_id);
  auto& create_subscription_result =
      *response.mutable_create_subscription_result();
  create_subscription_result.set_subscription_id(subscription_id);
  Convert(scada::Status{scada::StatusCode::Good}, *response.mutable_status());

  Send(message);
}

void SessionStub::OnDeleteSubscription(int request_id, int subscription_id) {
  auto result = subscriptions_.erase(subscription_id)
                    ? scada::StatusCode::Good
                    : scada::StatusCode::Bad_WrongSubscriptionId;

  protocol::Message message;
  auto& response = *message.add_responses();
  response.set_request_id(request_id);
  Convert(scada::Status{result}, *response.mutable_status());

  Send(message);
}

void SessionStub::OnCreateMonitoredItem(int request_id,
                                        int subscription_id,
                                        scada::ReadValueId&& read_value_id,
                                        scada::MonitoringParameters&& params) {
  auto i = subscriptions_.find(subscription_id);
  if (i != subscriptions_.end()) {
    i->second->OnCreateMonitoredItem(request_id, std::move(read_value_id),
                                     std::move(params));
  }
}

void SessionStub::OnDeleteMonitoredItem(int request_id,
                                        int subscription_id,
                                        int monitored_item_id) {
  auto i = subscriptions_.find(subscription_id);
  if (i != subscriptions_.end())
    i->second->OnDeleteMonitoredItem(request_id, monitored_item_id);
}

void SessionStub::SendResponse(protocol::Response& response) {
  protocol::Message message;
  message.add_responses()->Swap(&response);
  Send(message);
}

void SessionStub::OnSessionDeleted() {
  protocol::Message message;
  message.add_notifications()->mutable_session_deleted();
  Send(message);
}

void SessionStub::OnCall(unsigned request_id,
                         const scada::NodeId& node_id,
                         const scada::NodeId& method_id,
                         const std::vector<scada::Variant>& arguments) {
  if (!services_.method_service) {
    protocol::Response response;
    response.set_request_id(request_id);
    Convert(scada::Status{scada::StatusCode::Bad},
            *response.mutable_status());
    SendResponse(response);
    return;
  }
  auto self = shared_from_this();
  CoSpawn(
      executor_,
      [self, request_id, node_id, method_id,
       arguments]() -> Awaitable<void> {
        co_await self->OnCallAsync(request_id, node_id, method_id, arguments);
      });
}

void SessionStub::OnRead(const protocol::Request& request) {
  if (!request.has_read())
    return;
  if (!services_.attribute_service) {
    protocol::Response response;
    response.set_request_id(request.request_id());
    Convert(scada::Status{scada::StatusCode::Bad},
            *response.mutable_status());
    SendResponse(response);
    return;
  }

  const auto inputs = std::make_shared<const std::vector<scada::ReadValueId>>(
      ConvertTo<std::vector<scada::ReadValueId>>(request.read().value_id()));
  scada::ServiceContext context =
      service_context_.with_trace_id(request.trace_id());
  auto self = shared_from_this();
  CoSpawn(
      executor_,
      [self, request_id = request.request_id(), context = std::move(context),
       inputs]() mutable -> Awaitable<void> {
        co_await self->OnReadAsync(request_id, std::move(context), inputs);
      });
}

void SessionStub::OnWrite(const protocol::Request& request) {
  if (request.write_size() == 0)
    return;
  if (!services_.attribute_service) {
    protocol::Response response;
    response.set_request_id(request.request_id());
    Convert(scada::Status{scada::StatusCode::Bad},
            *response.mutable_status());
    SendResponse(response);
    return;
  }

  const auto request_id = request.request_id();
  const auto inputs = std::make_shared<const std::vector<scada::WriteValue>>(
      ConvertTo<std::vector<scada::WriteValue>>(request.write()));
  auto self = shared_from_this();
  CoSpawn(
      executor_,
      [self, request_id, inputs]() -> Awaitable<void> {
        co_await self->OnWriteAsync(request_id, inputs);
      });
}

Awaitable<void> SessionStub::OnCallAsync(
    unsigned request_id,
    scada::NodeId node_id,
    scada::NodeId method_id,
    std::vector<scada::Variant> arguments) {
  auto status =
      co_await scada::CallAsync(executor_, *services_.method_service,
                                std::move(node_id), std::move(method_id),
                                std::move(arguments), service_context_.user_id());

  if (!connection_)
    co_return;

  protocol::Message message;
  auto& response = *message.add_responses();
  response.set_request_id(request_id);
  Convert(status, *response.mutable_status());
  Send(message);
}

Awaitable<void> SessionStub::OnReadAsync(
    unsigned request_id,
    scada::ServiceContext context,
    std::shared_ptr<const std::vector<scada::ReadValueId>> inputs) {
  auto [status, results] = co_await scada::ReadAsync(
      executor_, *services_.attribute_service, std::move(context),
      std::move(inputs));

  if (!connection_)
    co_return;

  LOG_INFO(logger_) << "Read async completed"
                    << LOG_TAG("RequestId", request_id)
                    << LOG_TAG("Status", ToString(status))
                    << LOG_TAG("InputCount", inputs ? inputs->size() : 0)
                    << LOG_TAG("ResultCount", results.size());

  protocol::Message message;
  auto& response = *message.add_responses();
  response.set_request_id(request_id);
  Convert(std::move(status), *response.mutable_status());
  Convert(std::move(results), *response.mutable_read_result()->mutable_value());
  Send(message);
}

Awaitable<void> SessionStub::OnWriteAsync(
    unsigned request_id,
    std::shared_ptr<const std::vector<scada::WriteValue>> inputs) {
  auto [status, status_codes] =
      co_await scada::WriteAsync(executor_, *services_.attribute_service,
                                 service_context_, std::move(inputs));

  if (!connection_)
    co_return;

  LOG_INFO(logger_) << "Write async completed"
                    << LOG_TAG("RequestId", request_id)
                    << LOG_TAG("Status", ToString(status))
                    << LOG_TAG("InputCount", inputs ? inputs->size() : 0)
                    << LOG_TAG("ResultCount", status_codes.size());

  protocol::Message message;
  auto& response = *message.add_responses();
  response.set_request_id(request_id);
  Convert(status, *response.mutable_status());
  Convert(std::move(status_codes), *response.mutable_write_result());
  Send(message);
}
