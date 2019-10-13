#include "remote/session_stub.h"

#include "core/attribute_service.h"
#include "core/event_service.h"
#include "core/method_service.h"
#include "core/write_flags.h"
#include "remote/connection.h"
#include "remote/history_stub.h"
#include "remote/node_management_stub.h"
#include "remote/protocol.h"
#include "remote/protocol_utils.h"
#include "remote/subscription_stub.h"
#include "remote/view_service_stub.h"

SessionStub::SessionStub(SessionContext&& context)
    : SessionContext(std::move(context)),
      // Can't use std::make_unique:
      // conversion from 'SessionStub *' to 'MessageSender &' exists, but is
      // inaccessible
      view_service_stub_{std::make_shared<ViewServiceStub>(
          ViewServiceStubContext{io_context_, *this, view_service_})},
      node_management_stub_{std::make_shared<NodeManagementStub>(
          static_cast<MessageSender&>(*this),
          node_management_service_,
          user_id_)},
      history_stub_{
          std::make_shared<HistoryStub>(history_service_,
                                        static_cast<MessageSender&>(*this),
                                        io_context_)} {
  LOG_INFO(logger_) << "Created";
}

SessionStub::~SessionStub() {
  LOG_INFO(logger_) << "Destroying";

  if (connection_)
    connection_->OnSessionDeleted();
}

std::shared_ptr<SessionStub> SessionStub::Create(SessionContext&& context) {
  return std::shared_ptr<SessionStub>(new SessionStub(std::move(context)));
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
    Convert(scada::StatusCode::Good, *response.mutable_status());
    SendResponse(response);
  }

  if (request.has_read()) {
    auto& read = request.read();
    OnRead(request.request_id(),
           ConvertTo<std::vector<scada::ReadValueId>>(read.value_id()));
  }

  if (!request.write().empty()) {
    std::vector<scada::WriteValue> values;
    values.reserve(request.write().size());
    for (auto& proto_value : request.write()) {
      scada::WriteFlags flags;
      if (proto_value.select())
        flags.set_select();
      values.emplace_back(scada::WriteValue{
          ConvertTo<scada::NodeId>(proto_value.node_id()),
          ConvertTo<scada::AttributeId>(proto_value.attribute_id()),
          ConvertTo<scada::Variant>(proto_value.value()), flags});
    }
    OnWrite(request.request_id(), values);
  }

  if (request.has_call()) {
    auto& call = request.call();
    if (call.has_acknowledge()) {
      auto& acknowledge = call.acknowledge();
      event_service_.Acknowledge(acknowledge.acknowledge_id(), user_id_);
      protocol::Response response;
      response.set_request_id(request.request_id());
      Convert(scada::StatusCode::Good, *response.mutable_status());
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
    OnCreateSubscription(request.request_id());
  }

  if (request.has_delete_subscription()) {
    auto& delete_subscription = request.delete_subscription();
    OnDeleteSubscription(request.has_create_subscription(),
                         delete_subscription.subscription_id());
  }

  if (request.has_create_monitored_item()) {
    auto& create_monitored_item = request.create_monitored_item();
    OnCreateMonitoredItem(
        request.request_id(), create_monitored_item.subscription_id(),
        {ConvertTo<scada::NodeId>(create_monitored_item.node_id()),
         static_cast<scada::AttributeId>(create_monitored_item.attribute_id())},
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

  LOG_INFO(logger_) << "Create subscription"
                    << LOG_TAG("RequestId", request_id);

  auto subscription_id = next_subscription_id_++;
  // Can't use std::make_unique:
  // conversion from 'SessionStub *' to 'MessageSender &' exists, but is
  // inaccessible
  std::unique_ptr<SubscriptionStub> subscription{new SubscriptionStub{
      *this, monitored_item_service_, subscription_id, params}};
  subscriptions_.emplace(subscription_id, std::move(subscription));

  const scada::Status status = scada::StatusCode::Good;

  LOG_STATUS(logger_, status)
      << "Create subscription completed" << LOG_TAG("RequestId", request_id)
      << LOG_TAG("SubscriptionId", subscription_id)
      << LOG_TAG("Status", ToString(status));

  protocol::Message message;
  auto& response = *message.add_responses();
  response.set_request_id(request_id);
  auto& create_subscription_result =
      *response.mutable_create_subscription_result();
  create_subscription_result.set_subscription_id(subscription_id);
  Convert(status, *response.mutable_status());
  Send(message);
}

void SessionStub::OnDeleteSubscription(int request_id, int subscription_id) {
  LOG_INFO(logger_) << "Delete subscription" << LOG_TAG("RequestId", request_id)
                    << LOG_TAG("SubscriptionId", subscription_id);

  const scada::Status status = subscriptions_.erase(subscription_id)
                                   ? scada::StatusCode::Good
                                   : scada::StatusCode::Bad_WrongSubscriptionId;

  LOG_STATUS(logger_, status)
      << "Delete subscription completed" << LOG_TAG("RequestId", request_id)
      << LOG_TAG("Status", ToString(status));

  protocol::Message message;
  auto& response = *message.add_responses();
  response.set_request_id(request_id);
  Convert(status, *response.mutable_status());
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
  LOG_INFO(logger_) << "Call" << LOG_TAG("RequestId", request_id)
                    << LOG_TAG("NodeId", ToString(node_id))
                    << LOG_TAG("MethodId", ToString(node_id));

  method_service_.Call(
      node_id, method_id, arguments, user_id_,
      io_context_.wrap([this, weak_ptr = weak_from_this(),
                        request_id](const scada::Status& status) {
        auto ptr = weak_ptr.lock();
        if (!ptr)
          return;

        LOG_STATUS(logger_, status)
            << "Call completed" << LOG_TAG("RequestId", request_id)
            << LOG_TAG("Status", ToString(status));

        protocol::Message message;
        auto& response = *message.add_responses();
        response.set_request_id(request_id);
        Convert(status, *response.mutable_status());
        Send(message);
      }));
}

void SessionStub::OnRead(
    unsigned request_id,
    const std::vector<scada::ReadValueId>& read_value_ids) {
  attribute_service_.Read(
      read_value_ids,
      io_context_.wrap(
          [this, weak_ptr = weak_from_this(), request_id](
              scada::Status status, std::vector<scada::DataValue> results) {
            auto ptr = weak_ptr.lock();
            if (!ptr)
              return;

            protocol::Message message;
            auto& response = *message.add_responses();
            response.set_request_id(request_id);
            Convert(std::move(status), *response.mutable_status());
            Convert(std::move(results),
                    *response.mutable_read_result()->mutable_value());
            Send(message);
          }));
}

void SessionStub::OnWrite(unsigned request_id,
                          base::span<const scada::WriteValue> values) {
  LOG_INFO(logger_) << "Write" << LOG_TAG("RequestId", request_id)
                    << LOG_TAG("Count", values.size());

  attribute_service_.Write(
      values, user_id_,
      io_context_.wrap(
          [this, weak_ptr = weak_from_this(), request_id](
              scada::Status status, std::vector<scada::Status> results) {
            auto ptr = weak_ptr.lock();
            if (!ptr)
              return;

            LOG_STATUS(logger_, status)
                << "Write completed" << LOG_TAG("RequestId", request_id)
                << LOG_TAG("Status", ToString(status));

            protocol::Message message;
            auto& response = *message.add_responses();
            response.set_request_id(request_id);
            Convert(status, *response.mutable_status());
            Convert(std::move(results), *response.mutable_write_result());
            Send(message);
          }));
}
