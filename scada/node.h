#pragma once

#include "base/awaitable.h"
#include "base/lifetime.h"
#include "scada/attribute_ids.h"
#include "scada/co_result.h"
#include "scada/history_service.h"
#include "scada/monitored_item_service.h"
#include "scada/service_context.h"
#include "scada/services.h"
#include "scada/status_or.h"
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

  const scada::NodeId& id() const SCADA_LIFETIME_BOUND { return node_id_; }
  const ServiceContext& context() const SCADA_LIFETIME_BOUND {
    return context_;
  }

  CoStatusOr<DataValue> read(AttributeId attribute_id) const;

  CoStatusOr<DataValue> read_value() const { return read(AttributeId::Value); }

  CoStatus write(AttributeId attribute_id,
                 Variant value,
                 scada::WriteFlags flags = {}) const;

  CoStatus write_value(const Variant& value,
                       scada::WriteFlags flags = {}) const {
    return write(AttributeId::Value, value, flags);
  }

  struct browse_details {
    NodeId reference_type_id = id::References;
    BrowseDirection direction = BrowseDirection::Both;
  };

  // The awaitable-returning methods below are lazy coroutines: value-type
  // parameters are taken by value so the coroutine frame owns copies. A
  // const& parameter bound to a caller temporary (e.g. the braced defaults
  // in `parent()` / `type_definition()`) would dangle, because the body
  // first runs after the caller's full expression has ended.
  //
  // The node object itself must outlive the returned awaitable: member
  // coroutines capture `this`, not a copy of the node. `co_await
  // client.node(id).browse()` in one full expression is safe (the temporary
  // lives in the awaiting coroutine's frame across suspension), but storing
  // the awaitable past the node's lifetime is not.
  CoStatusOr<std::vector<ReferenceDescription>> browse(
      browse_details details = browse_details{
          .reference_type_id = id::References,
          .direction = BrowseDirection::Both}) const;

  CoStatusOr<scada::node> browse_node(
      browse_details details = browse_details{
          .reference_type_id = id::References,
          .direction = BrowseDirection::Both}) const;

  CoStatusOr<scada::node> parent() const {
    return browse_node({.reference_type_id = id::HierarchicalReferences,
                        .direction = BrowseDirection::Inverse});
  }

  CoStatusOr<scada::node> type_definition() const {
    return browse_node({.reference_type_id = id::HasTypeDefinition,
                        .direction = BrowseDirection::Forward});
  }

  // Takes vector instead of span as a parameter to simplify invocation.
  // Requires `ViewService`.
  CoStatusOr<std::vector<BrowsePathTarget>> translate_browse_path(
      RelativePath relative_path) const;

  // Requires `ViewService`.
  CoStatusOr<NodeId> child_id(scada::QualifiedName browse_name) const;
  CoStatusOr<node> child_node(scada::QualifiedName browse_name) const;

  CoStatus call_packed(NodeId method_id, std::vector<Variant> arguments) const;

  template <class... Args>
  CoStatus call(NodeId method_id, Args&&... args) const {
    return call_packed(std::move(method_id), {std::forward<Args>(args)...});
  }

  // `details.node_id` is overridden by the node ID and doesn't have
  // to be set.
  CoStatusOr<std::vector<scada::DataValue>> read_value_history(
      HistoryReadRawDetails details) const;

  // `details.node_id` is overridden by the node ID and doesn't have
  // to be set.
  CoStatusOr<HistoryReadRawResult> read_value_history_chunk(
      HistoryReadRawDetails details) const;

  struct event_history_details {
    Time from = scada::kNullTime;
    Time to = scada::kNullTime;
    EventFilter filter;
  };

  CoStatusOr<std::vector<Event>> read_event_history(
      event_history_details details) const;
  // Convenience overload for an unbounded query (from/to default to the null
  // sentinel). A defaulted argument cannot be used here because the nested
  // struct carries default member initializers.
  CoStatusOr<std::vector<Event>> read_event_history() const;

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
