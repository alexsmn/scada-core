#pragma once

#include "base/any_executor.h"
#include "base/any_executor_timer.h"
#include "base/async_completion.h"
#include "base/awaitable.h"
#include "base/boost_log.h"
#include "base/cancelation.h"
#include "remote/message_sender.h"
#include "scada/attribute_service.h"
#include "scada/logging.h"
#include "scada/method_service.h"
#include "scada/monitored_item_service.h"
#include "scada/privileges.h"
#include "scada/services.h"
#include "scada/session_service.h"
#include "scada/status.h"

#include <boost/signals2/signal.hpp>
#include <optional>
#include <transport/any_transport.h>
#include <transport/write_queue.h>
#include <unordered_map>

namespace transport {
class TransportFactory;
}  // namespace transport

namespace scada {
class NodeManagementService;
class HistoryService;
class ViewService;
}  // namespace scada

class NodeManagementProxy;
class HistoryProxy;
class SubscriptionProxy;
class ViewServiceProxy;
class SessionProxyDebugger;

struct SessionProxyContext {
  const AnyExecutor executor_;
  transport::TransportFactory& transport_factory_;
  const scada::ServiceLogParams service_log_params_;
};

class SessionProxy : private SessionProxyContext,
                     private scada::AttributeService,
                     private scada::MethodService,
                     private scada::MonitoredItemService,
                     private scada::SessionService,
                     public MessageSender {
 public:
  explicit SessionProxy(SessionProxyContext&& context);
  virtual ~SessionProxy();

  [[nodiscard]] scada::services services();

  // scada::SessionService
  virtual Awaitable<void> Connect(scada::SessionConnectParams params) override;
  virtual Awaitable<scada::Status> ConnectStatus(
      scada::SessionConnectParams params) override;
  virtual Awaitable<void> Reconnect() override;
  virtual Awaitable<void> Disconnect() override;
  virtual bool IsConnected(scada::base::TimeDelta* ping_delay) const override;
  virtual bool HasPrivilege(scada::Privilege privilege) const override;
  virtual bool IsScada() const override { return false; }
  virtual scada::NodeId GetUserId() const override;
  virtual std::string GetHostName() const override;
  virtual boost::signals2::scoped_connection SubscribeSessionStateChanged(
      const SessionStateChangedCallback& callback) override;
  virtual scada::SessionDebugger* GetSessionDebugger() override;

  // MessageSender
  virtual void Send(protocol::Message& message) override;
  virtual void Request(protocol::Request& request,
                       ResponseHandler response_handler) override;

  // scada::MonitoredItemService
  scada::StatusOr<std::unique_ptr<scada::MonitoredItemSubscription>>
  CreateSubscription(scada::ServiceContext context,
                     scada::MonitoredItemSubscriptionOptions options) override;

  // scada::AttributeService
  [[nodiscard]] virtual Awaitable<
      scada::StatusOr<std::vector<scada::DataValue>>>
  Read(scada::ServiceContext context,
       std::vector<scada::ReadValueId> inputs) override;
  [[nodiscard]] virtual Awaitable<
      scada::StatusOr<std::vector<scada::StatusCode>>>
  Write(scada::ServiceContext context,
        std::vector<scada::WriteValue> inputs) override;

  // scada::MethodService
  virtual Awaitable<scada::Status> Call(scada::NodeId node_id,
                                        scada::NodeId method_id,
                                        std::vector<scada::Variant> arguments,
                                        scada::ServiceContext context) override;

 private:
  [[nodiscard]] transport::awaitable<void> Connect();
  [[nodiscard]] Awaitable<scada::Status> ConnectAsync(
      scada::SessionConnectParams params);
  [[nodiscard]] Awaitable<void> ReconnectAsync();

  void OnSessionError(const scada::Status& status);

  void OnSessionCreated();
  void OnSessionDeleted();

  void OnMessageReceived(const protocol::Message& message);

  [[nodiscard]] Awaitable<void> AwaitCreateSessionAsync();
  [[nodiscard]] Awaitable<scada::Status> DisconnectAsync();
  [[nodiscard]] Awaitable<void> PingAsync();
  [[nodiscard]] Awaitable<protocol::Response> RequestAsync(
      protocol::Request request);

  void ForwardConnectResult(scada::Status&& status);

  void SchedulePing();
  void Ping();

  bool IsMessageLogged(const protocol::Message& message) const;

  void OnTransportOpened();
  void OnTransportClosed(transport::error_code error);
  void OnTransportMessageReceived(std::span<const char> data);

  const std::shared_ptr<BoostLogger> logger_ =
      std::make_shared<BoostLogger>(LOG_NAME("SessionProxy"));

  transport::any_transport transport_;

  const std::unique_ptr<SessionProxyDebugger> debugger_;

  const std::shared_ptr<SubscriptionProxy> subscription_;
  const std::unique_ptr<ViewServiceProxy> view_service_proxy_;
  const std::unique_ptr<NodeManagementProxy> node_management_proxy_;
  const std::unique_ptr<HistoryProxy> history_proxy_;

  bool session_created_ = false;

  scada::LocalizedText user_name_;
  scada::LocalizedText password_;
  std::string host_;
  std::string connection_string_;
  bool allow_remote_logoff_ = false;

  scada::NodeId user_node_id_;
  unsigned user_rights_ = 0;
  std::string session_token_;

  std::unordered_map<int /*request_id*/, ResponseHandler> requests_;

  boost::signals2::signal<void(bool connected, const scada::Status& status)>
      session_state_changed_signal_;

  scada::base::AsyncCompletion connect_completion_;
  scada::base::AsyncCompletion connect_loop_completion_;
  scada::base::AsyncCompletion ping_completion_;
  std::optional<scada::Status> pending_connect_result_;
  scada::Status connect_status_ = scada::StatusCode::Good;

  int next_request_id_ = 1;

  // Ping.
  AnyExecutorTimer ping_timer_;
  scada::base::TimeTicks ping_time_;
  scada::base::TimeDelta last_ping_delay_;

  std::optional<transport::WriteQueue> write_queue_;

  Cancelation cancelation_;
  std::shared_ptr<void> lifetime_ = std::make_shared<bool>();

  friend class EventServiceProxy;
};
