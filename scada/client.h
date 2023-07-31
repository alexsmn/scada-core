#pragma once

#include "scada/attribute_service_promises.h"
#include "scada/history_service.h"
#include "scada/method_service_promises.h"
#include "scada/monitored_item.h"
#include "scada/monitored_item_service.h"
#include "scada/session_service.h"
#include "scada/view_service.h"

namespace scada {

class client;
class node;
struct AddNodesItem;

class monitored_item {
 public:
  monitored_item() = default;

  bool subscribed() const { return state_ != nullptr; }

  void unsubscribe() { state_ = nullptr; }

 private:
  template <class Handler>
  void subscribe(std::shared_ptr<MonitoredItem> monitored_item,
                 Handler&& data_change_handler) {
    assert(!state_);
    assert(monitored_item);

    state_ = std::make_shared<state>(monitored_item);

    monitored_item->Subscribe(
        [weak_state = std::weak_ptr{state_},
         data_change_handler = std::forward<Handler>(data_change_handler)](
            const DataValue& data_value) mutable {
          if (auto state = weak_state.lock()) {
            state->handle_status(data_value.status_code);
            data_change_handler(data_value);
          }
        });
  }

  template <class Handler>
  void subscribe_events(std::shared_ptr<MonitoredItem> monitored_item,
                        Handler&& event_handler) {
    assert(!state_);
    assert(monitored_item);

    state_ = std::make_shared<state>(monitored_item);

    monitored_item->Subscribe(
        [weak_state = std::weak_ptr{state_},
         event_handler = std::forward<Handler>(event_handler)](
            const Status& status, const std::any& event) mutable {
          if (auto state = weak_state.lock()) {
            state->handle_status(status.code());
            event_handler(status, event);
          }
        });
  }

  struct state {
    explicit state(std::shared_ptr<MonitoredItem> monitored_item);

    void handle_status(scada::StatusCode status_code);

    // Monitored item is reset only once when bad status code is received. It's
    // safe to keep it outside of mutex.
    std::shared_ptr<MonitoredItem> monitored_item_;
  };

  std::shared_ptr<state> state_;

  friend class node;
};

class node {
 public:
  const ServiceContext& context() const { return *context_; }

  node() = default;

  node with_context(ServiceContext context) const {
    return node{services_, node_id_,
                std::make_shared<ServiceContext>(std::move(context))};
  }

  const scada::NodeId& id() const { return node_id_; }

  promise<DataValue> read(AttributeId attribute_id) const;

  promise<DataValue> read_value() const { return read(AttributeId::Value); }

  promise<> write(AttributeId attribute_id,
                  const Variant& value,
                  scada::WriteFlags flags = {}) const;

  promise<> write_value(const Variant& value,
                        scada::WriteFlags flags = {}) const {
    return write(AttributeId::Value, value, flags);
  }

  struct browse_details {
    NodeId reference_type_id = id::References;
    BrowseDirection direction = BrowseDirection::Both;
  };

  promise<std::vector<ReferenceDescription>> browse(
      const browse_details& details = {}) const;

  promise<scada::node> browse_node(const browse_details& details = {}) const;

  promise<scada::node> parent() const {
    return browse_node({.reference_type_id = id::HierarchicalReferences,
                        .direction = BrowseDirection::Inverse});
  }

  promise<scada::node> type_definition() const {
    return browse_node({.reference_type_id = id::HasTypeDefinition,
                        .direction = BrowseDirection::Forward});
  }

  // Takes vector instead of span as a parameter to simplify invocation.
  // Requires `ViewService`.
  promise<std::vector<BrowsePathTarget>> translate_browse_path(
      const RelativePath& relative_path) const;

  // Requires `ViewService`.
  promise<NodeId> child_id(scada::QualifiedName browse_name) const;
  promise<node> child_node(scada::QualifiedName browse_name) const;

  promise<> call_packed(const NodeId& method_id,
                        const std::vector<Variant>& arguments) const;

  template <class... Args>
  promise<> call(const NodeId& method_id, Args&&... args) const {
    return call_packed(method_id, {std::forward<Args>(args)...});
  }

  // `details.node_id` is overridden by the node ID and doesn't have
  // to be set.
  promise<std::vector<scada::DataValue>> read_value_history(
      const HistoryReadRawDetails& details) const;

  // `details.node_id` is overridden by the node ID and doesn't have
  // to be set. Returns either good `HistoryReadRawResult.status_code` or
  // rejected status promise.
  promise<HistoryReadRawResult> read_value_history_chunk(
      const HistoryReadRawDetails& details) const;

  struct event_history_details {
    DateTime from;
    DateTime to;
    EventFilter filter;
  };

  promise<std::vector<Event>> read_event_history(
      const event_history_details& details = {}) const;

  template <class Handler>
  monitored_item subscribe(AttributeId attribute_id,
                           Handler&& data_change_handler) const;

  template <class Handler>
  monitored_item subscribe_value(Handler&& data_change_handler) const {
    return subscribe(AttributeId::Value,
                     std::forward<Handler>(data_change_handler));
  }

  template <class Handler>
  monitored_item subscribe_events(const EventFilter& filter,
                                  Handler&& event_handler) const;

  template <class Handler>
  monitored_item subscribe_events(Handler&& event_handler) const {
    return subscribe_events({}, std::forward<Handler>(event_handler));
  }

 private:
  node(services services, NodeId node_id, ServiceContextPtr context)
      : services_{std::move(services)},
        node_id_{std::move(node_id)},
        context_{std::move(context)} {
    assert(context_);
  }

  const services services_;
  const NodeId node_id_;
  const ServiceContextPtr context_ = ServiceContext::default_instance();

  friend class client;
};

class client {
 public:
  client() {}
  explicit client(services services) : services_{std::move(services)} {}

  const ServiceContext& context() const { return *context_; }

  client with_context(ServiceContext context) const {
    return client{services_,
                  std::make_shared<ServiceContext>(std::move(context))};
  }

  promise<> connect(const SessionConnectParams& params) const;

  promise<> disconnect() const;

  scada::node node(const NodeId& node_id) const {
    assert(!node_id.is_null());
    return scada::node{services_, node_id, context_};
  }

  scada::node server_node() const { return node(id::Server); }

  promise<scada::node> add_node(const AddNodesItem& item);

  template <class Handler>
  monitored_item subscribe_events(const scada::EventFilter& filter,
                                  Handler&& handler) const {
    return server_node().subscribe_events(filter,
                                          std::forward<Handler>(handler));
  }

  template <class Handler>
  monitored_item subscribe_events(Handler&& handler) const {
    return server_node().subscribe_events(std::forward<Handler>(handler));
  }

  promise<> acknowledge_events(std::vector<EventAcknowledgeId> event_ids,
                               DateTime acknowledge_time) const;

  promise<> acknowledge_event(EventAcknowledgeId event_id,
                              DateTime acknowledge_time) const {
    return acknowledge_events({event_id}, acknowledge_time);
  }

 private:
  client(services services, std::shared_ptr<ServiceContext> context)
      : services_{std::move(services)}, context_{std::move(context)} {
    assert(context_);
  }

  services services_;
  ServiceContextPtr context_ = ServiceContext::default_instance();
};

template <class Handler>
inline monitored_item node::subscribe(AttributeId attribute_id,
                                      Handler&& data_change_handler) const {
  assert(attribute_id != AttributeId::EventNotifier);

  if (!services_.monitored_item_service) {
    data_change_handler(MakeReadError(StatusCode::Bad_Disconnected));
    return {};
  }

  auto item = services_.monitored_item_service->CreateMonitoredItem(
      {node_id_, attribute_id}, {});

  if (!item) {
    data_change_handler(MakeReadError(StatusCode::Bad_WrongNodeId));
    return {};
  }

  monitored_item result;
  result.subscribe(std::move(item), std::forward<Handler>(data_change_handler));

  return result;
}

template <class Handler>
inline monitored_item node::subscribe_events(const EventFilter& filter,
                                             Handler&& event_handler) const {
  if (!services_.monitored_item_service) {
    // Must specify `std::any` to avoid ambiguity when using `BindExecutor`.
    event_handler(StatusCode::Bad_Disconnected, std::any{});
    return {};
  }

  auto item = services_.monitored_item_service->CreateMonitoredItem(
      {.node_id = node_id_, .attribute_id = AttributeId::EventNotifier},
      {.filter = filter});

  if (!item) {
    // Must specify `std::any` to avoid ambiguity when using `BindExecutor`.
    event_handler(StatusCode::Bad_WrongNodeId, std::any{});
    return {};
  }

  monitored_item result;
  result.subscribe_events(std::move(item),
                          std::forward<Handler>(event_handler));

  return result;
}

}  // namespace scada
