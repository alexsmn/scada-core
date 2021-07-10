#include "remote/session_stub.h"

#include "base/executor.h"
#include "base/range_util.h"
#include "core/attribute_service.h"
#include "core/event_service.h"
#include "core/method_service.h"
#include "core/write_flags.h"
#include "model/node_id_util.h"
#include "remote/connection.h"
#include "remote/history_stub.h"
#include "remote/node_management_stub.h"
#include "remote/protocol.h"
#include "remote/protocol_utils.h"
#include "remote/subscription_stub.h"
#include "remote/view_service_stub.h"

SessionStub::SessionStub(SessionContext&& context)
    : SessionContext(std::move(context)) {
  LOG_BIND_TAG(logger_, "UserId",
               NodeIdToScadaString(service_context_->user_id));
  LOG_INFO(logger_) << "Created";
}

SessionStub::~SessionStub() {
  LOG_INFO(logger_) << "Destroying";

  if (connection_)
    connection_->OnSessionDeleted();
}

void SessionStub::Init() {
  const std::weak_ptr<MessageSender> sender = weak_from_this();

  view_service_stub_ = std::make_shared<ViewServiceStub>(
      ViewServiceStubContext{executor_, sender, view_service_});

  node_management_stub_ = std::make_shared<NodeManagementStub>(
      executor_, sender, node_management_service_, service_context_);

  history_stub_ =
      std::make_shared<HistoryStub>(history_service_, sender, executor_);
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
    if (call.has_acknowledge()) {
      auto& acknowledge = call.acknowledge();
      event_service_.Acknowledge(MakeVector<int>(acknowledge.acknowledge_id()),
                                 service_context_->user_id);
      protocol::Response response;
      response.set_request_id(request.request_id());
      Convert(scada::Status{scada::StatusCode::Good},
              *response.mutable_status());
      SendResponse(response);
    }
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
    scada::ReadValueId read_value_id{
        ConvertTo<scada::NodeId>(create_monitored_item.node_id()),
        static_cast<scada::AttributeId>(create_monitored_item.attribute_id()),
    };
    OnCreateMonitoredItem(
        request.request_id(), create_monitored_item.subscription_id(),
        std::move(read_value_id),
        create_monitored_item.has_monitoring_parameters()
            ? ConvertTo<scada::MonitoringParameters>(
                  create_monitored_item.monitoring_parameters())
            : scada::MonitoringParameters{});
  }

  if (request.has_delete_monitored_item()) {
    auto& delete_monitored_item = request.delete_monitored_item();
    OnDeleteMonitoredItem(request.request_id(),
                          delete_monitored_item.subscription_id(),
                          delete_monitored_item.monitored_item_id());
  }

  view_service_stub_->OnRequestReceived(request);
  node_management_stub_->OnRequestReceived(request);
  history_stub_->OnRequestReceived(request);
}

void SessionStub::Send(protocol::Message& message) {
  if (connection_)
    connection_->Send(message);
  else {
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
      executor_, sender, monitored_item_service_, subscription_id, params);
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
  std::weak_ptr<SessionStub> weak_ptr = shared_from_this();
  method_service_.Call(
      node_id, method_id, arguments, service_context_->user_id,
      BindExecutor(executor_,
                   [weak_ptr, request_id](const scada::Status& status) {
                     auto ptr = weak_ptr.lock();
                     if (!ptr)
                       return;

                     protocol::Message message;
                     auto& response = *message.add_responses();
                     response.set_request_id(request_id);
                     Convert(status, *response.mutable_status());
                     ptr->Send(message);
                   }));
}

void SessionStub::OnRead(const protocol::Request& request) {
  if (!request.has_read())
    return;

  const auto request_id = request.request_id();
  const auto inputs = std::make_shared<const std::vector<scada::ReadValueId>>(
      ConvertTo<std::vector<scada::ReadValueId>>(request.read().value_id()));

  attribute_service_.Read(
      service_context_, inputs,
      BindExecutor(executor_, [weak_ptr = weak_from_this(), request_id](
                                  scada::Status status,
                                  std::vector<scada::DataValue> results) {
        auto ptr = weak_ptr.lock();
        if (!ptr)
          return;

        protocol::Message message;
        auto& response = *message.add_responses();
        response.set_request_id(request_id);
        Convert(std::move(status), *response.mutable_status());
        Convert(std::move(results),
                *response.mutable_read_result()->mutable_value());
        ptr->Send(message);
      }));
}

void SessionStub::OnWrite(const protocol::Request& request) {
  if (request.write_size() == 0)
    return;

  const auto request_id = request.request_id();
  const auto inputs = std::make_shared<const std::vector<scada::WriteValue>>(
      ConvertTo<std::vector<scada::WriteValue>>(request.write()));

  attribute_service_.Write(
      service_context_, inputs,
      BindExecutor(executor_, [weak_ptr = weak_from_this(), request_id](
                                  scada::Status status,
                                  std::vector<scada::StatusCode> status_codes) {
        auto ptr = weak_ptr.lock();
        if (!ptr)
          return;

        protocol::Message message;
        auto& response = *message.add_responses();
        response.set_request_id(request_id);
        Convert(status, *response.mutable_status());
        Convert(std::move(status_codes), *response.mutable_write_result());
        ptr->Send(message);
      }));
}
