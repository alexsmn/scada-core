#include "scada/node.h"

#include "base/check.h"
#include "scada/attribute_service.h"
#include "scada/co_result.h"
#include "scada/method_service.h"
#include "scada/service_context.h"

#include <memory>

namespace scada {
namespace {

CoStatus WriteNodeAsync(services services,
                        NodeId node_id,
                        ServiceContext context,
                        AttributeId attribute_id,
                        Variant value,
                        scada::WriteFlags flags) {
  if (!services.attribute_service) {
    co_return StatusCode::Bad;
  }

  std::vector<WriteValue> write_values{{.node_id = std::move(node_id),
                                        .attribute_id = attribute_id,
                                        .value = std::move(value),
                                        .flags = flags}};
  auto statuses = co_await services.attribute_service->Write(
      context, std::move(write_values));
  if (!statuses.ok()) {
    co_return statuses.status();
  }
  base::Check(statuses->size() == 1);
  co_return (*statuses)[0];
}

CoStatusOr<CallResult> CallNodeAsync(services services,
                                     NodeId node_id,
                                     ServiceContext context,
                                     NodeId method_id,
                                     std::vector<Variant> arguments) {
  if (!services.method_service) {
    co_return StatusCode::Bad_Disconnected;
  }

  co_return co_await services.method_service->Call(
      std::move(node_id), std::move(method_id), std::move(arguments),
      std::move(context));
}

// Reduces a Call to its status. Free rather than a `node` member so that
// `call_packed` can stay a non-coroutine: see the comment there.
CoStatus CallStatusAsync(CoStatusOr<CallResult> operation) {
  co_return (co_await std::move(operation)).status();
}

CoStatusOr<DataValue> ReadNodeAsync(services services,
                                    NodeId node_id,
                                    ServiceContext context,
                                    AttributeId attribute_id) {
  if (!services.attribute_service) {
    co_return StatusCode::Bad_Disconnected;
  }

  std::vector<ReadValueId> read_values{
      {.node_id = std::move(node_id), .attribute_id = attribute_id}};
  auto results = co_await services.attribute_service->Read(
      std::move(context), std::move(read_values));
  if (!results.ok()) {
    co_return results.status();
  }
  base::Check(results->size() == 1);
  if (!IsGood((*results)[0].status_code)) {
    co_return (*results)[0].status_code;
  }
  co_return std::move((*results)[0]);
}

}  // namespace

node::node() = default;

node node::with_context(const ServiceContext& context) const {
  return node{services_, node_id_, context};
}

CoStatusOr<DataValue> node::read(AttributeId attribute_id) const {
  return ReadNodeAsync(services_, node_id_, context_, attribute_id);
}

CoStatus node::write(AttributeId attribute_id,
                     Variant value,
                     scada::WriteFlags flags) const {
  return WriteNodeAsync(services_, node_id_, context_, attribute_id,
                        std::move(value), flags);
}

CoStatusOr<std::vector<ReferenceDescription>> node::browse(
    browse_details details) const {
  if (!services_.view_service) {
    co_return StatusCode::Bad_Disconnected;
  }

  std::vector<BrowseDescription> nodes{
      {.node_id = node_id_,
       .direction = details.direction,
       .reference_type_id = details.reference_type_id}};
  auto results =
      co_await services_.view_service->Browse(context_, std::move(nodes));
  if (!results.ok()) {
    co_return results.status();
  }
  base::Check(results->size() == 1);
  if (!IsGood((*results)[0].status_code)) {
    co_return (*results)[0].status_code;
  }
  co_return std::move((*results)[0].references);
}

CoStatusOr<scada::node> node::browse_node(browse_details details) const {
  auto results = co_await browse(std::move(details));
  if (!results.ok()) {
    co_return results.status();
  }
  if (results->size() != 1) {
    co_return StatusCode::Bad;
  }
  co_return scada::node{services_, (*results)[0].node_id, context_};
}

CoStatusOr<std::vector<BrowsePathTarget>> node::translate_browse_path(
    RelativePath relative_path) const {
  if (!services_.view_service) {
    co_return StatusCode::Bad_Disconnected;
  }

  std::vector<BrowsePath> paths{
      {.node_id = node_id_, .relative_path = std::move(relative_path)}};
  auto results =
      co_await services_.view_service->TranslateBrowsePaths(std::move(paths));
  if (!results.ok()) {
    co_return results.status();
  }
  base::Check(results->size() == 1);
  if (!IsGood((*results)[0].status_code)) {
    co_return (*results)[0].status_code;
  }
  co_return std::move((*results)[0].targets);
}

CoStatusOr<NodeId> node::child_id(scada::QualifiedName browse_name) const {
  RelativePath path{{.reference_type_id = id::HasChild,
                     .target_name = std::move(browse_name)}};
  auto targets = co_await translate_browse_path(path);
  if (!targets.ok()) {
    co_return targets.status();
  }
  if (targets->size() != 1) {
    co_return StatusCode::Bad_BrowseNameInvalid;
  }
  co_return (*targets)[0].target_id.node_id();
}

CoStatusOr<node> node::child_node(scada::QualifiedName browse_name) const {
  auto id = co_await child_id(std::move(browse_name));
  if (!id.ok()) {
    co_return id.status();
  }
  co_return scada::node{services_, *id, context_};
}

CoStatus node::call_packed(NodeId method_id,
                           std::vector<Variant> arguments) const {
  // Deliberately NOT a coroutine. A coroutine member captures `this`, not a
  // copy of the node, and its body first runs when the awaitable is awaited —
  // so `SomeNode().call(...)`, whose node is a temporary, resumed a frame
  // pointing at freed memory and dereferenced a null `services_`. Delegating
  // eagerly snapshots services/node id/context at call time, the way `read()`
  // and `write()` already do, which makes the leaf operations uniformly safe
  // on a temporary node.
  return CallStatusAsync(
      call_packed_result(std::move(method_id), std::move(arguments)));
}

CoStatusOr<CallResult> node::call_packed_result(
    NodeId method_id,
    std::vector<Variant> arguments) const {
  return CallNodeAsync(services_, node_id_, context_, std::move(method_id),
                       std::move(arguments));
}

CoStatusOr<std::vector<scada::DataValue>> node::read_value_history(
    HistoryReadRawDetails details) const {
  base::Check(details.node_id.is_null());
  base::Check(details.continuation_point.empty());

  std::vector<scada::DataValue> values;
  auto next_details = std::move(details);

  for (;;) {
    auto result = co_await read_value_history_chunk(next_details);
    if (!result.ok()) {
      co_return result.status();
    }
    values.insert(values.end(), result->values.begin(), result->values.end());
    if (result->continuation_point.empty()) {
      co_return values;
    }
    next_details.continuation_point = result->continuation_point;
  }
}

CoStatusOr<HistoryReadRawResult> node::read_value_history_chunk(
    HistoryReadRawDetails details) const {
  base::Check(details.node_id.is_null());

  if (!services_.history_service) {
    co_return StatusCode::Bad_Disconnected;
  }

  auto sanitized_details = std::move(details);
  sanitized_details.node_id = node_id_;
  co_return co_await services_.history_service->HistoryReadRaw(
      std::move(sanitized_details));
}

CoStatusOr<std::vector<Event>> node::read_event_history(
    event_history_details details) const {
  if (!services_.history_service) {
    co_return StatusCode::Bad_Disconnected;
  }

  auto result = co_await services_.history_service->HistoryReadEvents(
      node_id_, details.from, details.to, details.filter);
  if (!result.ok()) {
    co_return result.status();
  }
  co_return std::move(result->events);
}

CoStatusOr<std::vector<Event>> node::read_event_history() const {
  return read_event_history(event_history_details{});
}

}  // namespace scada
