#pragma once

#include "base/boost_log.h"
#include "base/executor_timer.h"
#include "scada/attribute_service.h"
#include "scada/logging.h"
#include "scada/method_service.h"
#include "scada/monitored_item_service.h"
#include "scada/privileges.h"
#include "scada/session_service.h"
#include "scada/status.h"
#include "net/transport.h"
#include "remote/message_sender.h"

#include <boost/signals2/signal.hpp>
#include <map>

namespace net {
class Logger;
class TransportFactory;
}  // namespace net

namespace scada {
class NodeManagementService;
class EventService;
class HistoryService;
class ViewService;
}  // namespace scada

class Executor;
class NodeManagementProxy;
class HistoryProxy;
class SubscriptionProxy;
class ViewServiceProxy;

struct SessionProxyContext {
  const std::shared_ptr<Executor> executor_;
  net::TransportFactory& transport_factory_;
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

  scada::services services();

  // scada::SessionService
  virtual promise<> Connect(const scada::SessionConnectParams& params) override;
  virtual promise<> Reconnect() override;
  virtual promise<> Disconnect() override;
  virtual bool IsConnected(base::TimeDelta* ping_delay) const override;
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
  virtual std::shared_ptr<scada::MonitoredItem> CreateMonitoredItem(
      const scada::ReadValueId& read_value_id,
      const scada::MonitoringParameters& params) override;

  // scada::AttributeService
  virtual void Read(
      const std::shared_ptr<const scada::ServiceContext>& context,
      const std::shared_ptr<const std::vector<scada::ReadValueId>>& inputs,
      const scada::ReadCallback& callback) override;
  virtual void Write(
      const std::shared_ptr<const scada::ServiceContext>& context,
      const std::shared_ptr<const std::vector<scada::WriteValue>>& inputs,
      const scada::WriteCallback& callback) override;

  // scada::MethodService
  virtual void Call(const scada::NodeId& node_id,
                    const scada::NodeId& method_id,
                    const std::vector<scada::Variant>& arguments,
                    const scada::NodeId& user_id,
                    const scada::StatusCallback& callback) override;

 private:
  friend class EventServiceProxy;

  promise<> Connect();

  void OnSessionError(const scada::Status& status);

  void OnSessionCreated();
  void OnSessionDeleted();

  void OnMessageReceived(const protocol::Message& message);
  void OnCreateSessionResult(const protocol::Response& response);

  void ForwardConnectResult(scada::Status&& status);

  void SchedulePing();
  void Ping();

  bool IsMessageLogged(const protocol::Message& message) const;

  void OnTransportOpened();
  void OnTransportClosed(net::Error error);
  void OnTransportMessageReceived(std::span<const char> data);

  const std::shared_ptr<BoostLogger> logger_ =
      std::make_shared<BoostLogger>(LOG_NAME("SessionProxy"));

  std::unique_ptr<net::Transport> transport_;

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

  std::map<int /*request_id*/, ResponseHandler> requests_;

  boost::signals2::signal<void(bool connected, const scada::Status& status)>
      session_state_changed_signal_;

  promise<> connect_promise_;

  int next_request_id_ = 1;

  // Ping.
  ExecutorTimer ping_timer_;
  base::TimeTicks ping_time_;
  base::TimeDelta last_ping_delay_;
};
