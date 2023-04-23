#pragma once

#include "core/attribute_service_promises.h"
#include "core/method_service_promises.h"
#include "core/monitored_item.h"
#include "core/monitored_item_service.h"

namespace scada::client {

class client;
class node;

struct services {
  AttributeService* attribute_service = nullptr;
  MonitoredItemService* monitored_item_service = nullptr;
  MethodService* method_service = nullptr;
};

class monitored_item {
 public:
  monitored_item() = default;

  bool subscribed() const { return state_ != nullptr; }
  StatusCode status_code() const {
    return state_ ? state_->status_code : StatusCode::Bad_Disconnected;
  }

  const DataValue& data_value() const {
    static const DataValue kEmptyDataValue;
    return state_ ? state_->data_value : kEmptyDataValue;
  }

  const std::any& last_event() const {
    static const std::any kEmptyEvent;
    return state_ ? state_->last_event : kEmptyEvent;
  }

  void unsubscribe() { state_ = nullptr; }

 private:
  template <class Handler>
  void subscribe(std::shared_ptr<MonitoredItem> monitored_item,
                 Handler&& data_change_handler) {
    assert(!state_);
    assert(monitored_item);

    state_ = std::make_shared<state>();
    state_->monitored_item = std::move(monitored_item);
    state_->monitored_item->Subscribe(
        [state = state_,
         data_change_handler = std::forward<Handler>(data_change_handler)](
            const DataValue& data_value) mutable {
          // TODO: Handle close.
          state->status_code = data_value.status_code;
          state->data_value = data_value;
          data_change_handler(data_value);
        });
  }

  template <class Handler>
  void subscribe_events(std::shared_ptr<MonitoredItem> monitored_item,
                        Handler&& event_handler) {
    assert(!state_);
    assert(monitored_item);

    state_ = std::make_shared<state>();
    state_->monitored_item = std::move(monitored_item);
    state_->monitored_item->Subscribe(
        [state = state_, event_handler = std::forward<Handler>(event_handler)](
            const Status& status, const std::any& event) {
          // TODO: Handle close.
          state->status_code = status.code();
          state->last_event = event;
          event_handler(status, event);
        });
  }

  struct state {
    // TODO: Custom state for events.
    StatusCode status_code = StatusCode::Good;
    DataValue data_value;
    std::any last_event;
    std::shared_ptr<MonitoredItem> monitored_item;
  };

  std::shared_ptr<state> state_;

  friend class node;
};

namespace internal {

struct no_handler {
  void operator()(const scada::DataValue&) const {}
  void operator()(const scada::Status&, const std::any&) const {}
};

}  // namespace internal

class node {
 public:
  promise<DataValue> read(AttributeId attribute_id) const {
    if (!services_.attribute_service) {
      return make_resolved_promise<DataValue>(MakeReadError(StatusCode::Bad));
    }

    return scada::Read(*services_.attribute_service,
                       ServiceContext::default_instance(),
                       scada::ReadValueId{node_id_});
  }

  promise<DataValue> read_value() const { return read(AttributeId::Value); }

  promise<Status> write(AttributeId attribute_id,
                        const Variant& value,
                        const ServiceContext& service_context = {}) const {
    if (!services_.attribute_service) {
      return make_resolved_promise<Status>(StatusCode::Bad);
    }

    //   TODO: Take `ServiceContext` from `client`.
    return scada::Write(
        *services_.attribute_service,
        std::make_shared<scada::ServiceContext>(service_context),
        WriteValue{node_id_, attribute_id, value});
  }

  promise<Status> write_value(
      const Variant& value,
      const ServiceContext& service_context = {}) const {
    return write(AttributeId::Value, value, service_context);
  }

  promise<Status> call_packed(const NodeId& method_id,
                              const std::vector<Variant>& arguments) const {
    if (!services_.method_service) {
      return make_resolved_promise<Status>(StatusCode::Bad);
    }

    // Take `user_id` from `client`.
    return scada::Call(*services_.method_service, node_id_, method_id,
                       arguments, {});
  }

  template <class... Args>
  promise<Status> call(const NodeId& method_id, Args&&... args) const {
    return call_packed(method_id, {std::forward<Args>(args)...});
  }

  template <class Handler>
  monitored_item subscribe(
      AttributeId attribute_id,
      Handler&& data_change_handler = internal::no_handler{}) const {
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
    result.subscribe(std::move(item),
                     std::forward<Handler>(data_change_handler));

    return result;
  }

  template <class Handler>
  monitored_item subscribe_value(
      Handler&& data_change_handler = internal::no_handler{}) const {
    return subscribe(AttributeId::Value,
                     std::forward<Handler>(data_change_handler));
  }

  template <class Handler>
  monitored_item subscribe_events(
      Handler&& event_handler = internal::no_handler{}) const {
    if (!services_.monitored_item_service) {
      event_handler(StatusCode::Bad_Disconnected, {});
      return {};
    }

    auto item = services_.monitored_item_service->CreateMonitoredItem(
        {node_id_, AttributeId::EventNotifier}, {});

    if (!item) {
      event_handler(StatusCode::Bad_WrongNodeId, {});
      return {};
    }

    monitored_item result;
    result.subscribe_events(std::move(item),
                            std::forward<Handler>(event_handler));

    return result;
  }

 private:
  node(services services, NodeId node_id)
      : services_{std::move(services)}, node_id_{std::move(node_id)} {}

  const services services_;
  const NodeId node_id_;

  friend class client;
};

class client {
 public:
  explicit client(services services) : services_{std::move(services)} {}

  node node(const NodeId& node_id) {
    return scada::client::node{services_, node_id};
  }

 private:
  const services services_;
};

}  // namespace scada::client
