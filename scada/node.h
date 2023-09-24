#pragma once

#include "scada/history_service.h"
#include "scada/monitored_item_service.h"
#include "scada/status_promise.h"
#include "scada/view_service.h"

namespace scada {

class client;
class monitored_item;
class node;

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
  friend class monitored_item;
};

}  // namespace scada