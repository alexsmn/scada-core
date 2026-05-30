#pragma once

#include "base/awaitable.h"
#include "scada/attribute_ids.h"
#include "scada/history_service.h"
#include "scada/monitored_item_service.h"
#include "scada/service_context.h"
#include "scada/services.h"
#include "scada/view_service.h"
#include "scada/write_flags.h"

namespace scada {

class ServiceContext;
class client;
class monitored_item;
class node;

class node {
 public:
  node();

  node with_context(const ServiceContext& context) const;

  const scada::NodeId& id() const { return node_id_; }
  const ServiceContext& context() const { return context_; }

  Awaitable<DataValue> read(AttributeId attribute_id) const;

  Awaitable<DataValue> read_value() const { return read(AttributeId::Value); }

  Awaitable<void> write(AttributeId attribute_id,
                        Variant value,
                        scada::WriteFlags flags = {}) const;

  Awaitable<void> write_value(const Variant& value,
                              scada::WriteFlags flags = {}) const {
    return write(AttributeId::Value, value, flags);
  }

  struct browse_details {
    NodeId reference_type_id = id::References;
    BrowseDirection direction = BrowseDirection::Both;
  };

  Awaitable<std::vector<ReferenceDescription>> browse(
      const browse_details& details = browse_details{
          .reference_type_id = id::References,
          .direction = BrowseDirection::Both}) const;

  Awaitable<scada::node> browse_node(
      const browse_details& details = browse_details{
          .reference_type_id = id::References,
          .direction = BrowseDirection::Both}) const;

  Awaitable<scada::node> parent() const {
    return browse_node({.reference_type_id = id::HierarchicalReferences,
                        .direction = BrowseDirection::Inverse});
  }

  Awaitable<scada::node> type_definition() const {
    return browse_node({.reference_type_id = id::HasTypeDefinition,
                        .direction = BrowseDirection::Forward});
  }

  // Takes vector instead of span as a parameter to simplify invocation.
  // Requires `ViewService`.
  Awaitable<std::vector<BrowsePathTarget>> translate_browse_path(
      const RelativePath& relative_path) const;

  // Requires `ViewService`.
  Awaitable<NodeId> child_id(scada::QualifiedName browse_name) const;
  Awaitable<node> child_node(scada::QualifiedName browse_name) const;

  Awaitable<void> call_packed(NodeId method_id,
                              std::vector<Variant> arguments) const;

  template <class... Args>
  Awaitable<void> call(const NodeId& method_id, Args&&... args) const {
    return call_packed(method_id, {std::forward<Args>(args)...});
  }

  // `details.node_id` is overridden by the node ID and doesn't have
  // to be set.
  Awaitable<std::vector<scada::DataValue>> read_value_history(
      const HistoryReadRawDetails& details) const;

  // `details.node_id` is overridden by the node ID and doesn't have
  // to be set. Throws on bad `HistoryReadRawResult.status`.
  Awaitable<HistoryReadRawResult> read_value_history_chunk(
      const HistoryReadRawDetails& details) const;

  struct event_history_details {
    DateTime from;
    DateTime to;
    EventFilter filter;
  };

  Awaitable<std::vector<Event>> read_event_history(
      const event_history_details& details = {}) const;

 private:
  node(const services& services,
       const NodeId& node_id,
       const ServiceContext& context)
      : services_{services}, node_id_{node_id}, context_{context} {}

  const services services_;
  const NodeId node_id_;
  const ServiceContext context_;

  friend class client;
  friend class monitored_item;
};

}  // namespace scada
