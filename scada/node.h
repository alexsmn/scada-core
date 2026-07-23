#pragma once

#include "base/awaitable.h"
#include "base/lifetime.h"
#include "scada/attribute_ids.h"
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

  Awaitable<StatusOr<DataValue>> read(AttributeId attribute_id) const;

  Awaitable<StatusOr<DataValue>> read_value() const {
    return read(AttributeId::Value);
  }

  Awaitable<Status> write(AttributeId attribute_id,
                          Variant value,
                          scada::WriteFlags flags = {}) const;

  Awaitable<Status> write_value(const Variant& value,
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
  Awaitable<StatusOr<std::vector<ReferenceDescription>>> browse(
      browse_details details = browse_details{
          .reference_type_id = id::References,
          .direction = BrowseDirection::Both}) const;

  Awaitable<StatusOr<scada::node>> browse_node(
      browse_details details = browse_details{
          .reference_type_id = id::References,
          .direction = BrowseDirection::Both}) const;

  Awaitable<StatusOr<scada::node>> parent() const {
    return browse_node({.reference_type_id = id::HierarchicalReferences,
                        .direction = BrowseDirection::Inverse});
  }

  Awaitable<StatusOr<scada::node>> type_definition() const {
    return browse_node({.reference_type_id = id::HasTypeDefinition,
                        .direction = BrowseDirection::Forward});
  }

  // Takes vector instead of span as a parameter to simplify invocation.
  // Requires `ViewService`.
  Awaitable<StatusOr<std::vector<BrowsePathTarget>>> translate_browse_path(
      RelativePath relative_path) const;

  // Requires `ViewService`.
  Awaitable<StatusOr<NodeId>> child_id(scada::QualifiedName browse_name) const;
  Awaitable<StatusOr<node>> child_node(scada::QualifiedName browse_name) const;

  Awaitable<Status> call_packed(NodeId method_id,
                                std::vector<Variant> arguments) const;

  template <class... Args>
  Awaitable<Status> call(NodeId method_id, Args&&... args) const {
    return call_packed(std::move(method_id), {std::forward<Args>(args)...});
  }

  // `details.node_id` is overridden by the node ID and doesn't have
  // to be set.
  Awaitable<StatusOr<std::vector<scada::DataValue>>> read_value_history(
      HistoryReadRawDetails details) const;

  // `details.node_id` is overridden by the node ID and doesn't have
  // to be set.
  Awaitable<HistoryReadRawResult> read_value_history_chunk(
      HistoryReadRawDetails details) const;

  struct event_history_details {
    DateTime from = base::kNullTime;
    DateTime to = base::kNullTime;
    EventFilter filter;
  };

  Awaitable<StatusOr<std::vector<Event>>> read_event_history(
      event_history_details details) const;
  // Convenience overload for an unbounded query (from/to default to the null
  // sentinel). A defaulted argument cannot be used here because the nested
  // struct carries default member initializers.
  Awaitable<StatusOr<std::vector<Event>>> read_event_history() const;

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
