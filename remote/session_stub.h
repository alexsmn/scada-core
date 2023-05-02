#pragma once

#include "base/boost_log.h"
#include "core/attribute_service.h"
#include "core/event.h"
#include "remote/message_sender.h"
#include "remote/subscription.h"

#include <memory>
#include <unordered_map>

namespace protocol {
class Message;
class Request;
class Response;
}  // namespace protocol

namespace scada {
struct MonitoringParameters;
}

class Connection;
class Executor;
class EventServiceStub;
class HistoryStub;
class NodeManagementStub;
class SessionStub;
class SubscriptionStub;
class ViewServiceStub;

struct SessionContext {
  const std::shared_ptr<Executor> executor_;
  // TODO: Use `scada::client`.
  scada::services services_;
  const std::shared_ptr<const scada::ServiceContext> service_context_;
};

class SessionStub : public MessageSender,
                    private SessionContext,
                    public std::enable_shared_from_this<SessionStub> {
 public:
  ~SessionStub();

  static std::shared_ptr<SessionStub> Create(SessionContext&& context);

  Connection* connection() { return connection_; }
  void SetConnection(Connection* connection);

  const std::shared_ptr<const scada::ServiceContext>& service_context() const {
    return service_context_;
  }

  void ProcessMessage(const protocol::Message& message);

  void OnSessionDeleted();

 private:
  explicit SessionStub(SessionContext&& context);

  void Init();

  void ProcessRequest(const protocol::Request& request);

  void OnCreateSubscription(int request_id);
  void OnDeleteSubscription(int request_id, int subscription_id);
  void OnCreateMonitoredItem(int request_id,
                             int subscription_id,
                             scada::ReadValueId&& read_value_id,
                             scada::MonitoringParameters&& params);
  void OnDeleteMonitoredItem(int request_id,
                             int subscription_id,
                             int monitored_item_id);

  void OnRead(const protocol::Request& request);
  void OnWrite(const protocol::Request& request);
  void OnCall(unsigned request_id,
              const scada::NodeId& node_id,
              const scada::NodeId& method_id,
              const std::vector<scada::Variant>& arguments);

  void SendResponse(protocol::Response& response);

  // MessageSender
  virtual void Send(protocol::Message& message) override;
  virtual void Request(protocol::Request& request,
                       ResponseHandler response_handler) {}

  BoostLogger logger_{LOG_NAME("SessionStub")};

  Connection* connection_ = nullptr;

  std::shared_ptr<ViewServiceStub> view_service_stub_;
  std::shared_ptr<NodeManagementStub> node_management_stub_;
  std::shared_ptr<HistoryStub> history_stub_;

  int next_subscription_id_ = 1;
  std::unordered_map<int /*subscription_id*/, std::shared_ptr<SubscriptionStub>>
      subscriptions_;

  std::unique_ptr<protocol::Message> send_message_;
};
