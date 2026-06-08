#include "scada/node.h"

#include "base/any_executor.h"
#include "scada/service_awaitable.h"
#include "scada/service_context.h"

#include <boost/asio/this_coro.hpp>
#include <memory>

namespace scada {
namespace {

Awaitable<Status> WriteNodeAsync(services services,
                                 NodeId node_id,
                                 ServiceContext context,
                                 AttributeId attribute_id,
                                 Variant value,
                                 scada::WriteFlags flags) {
  if (!services.attribute_service) {
    co_return StatusCode::Bad;
  }

  std::vector<WriteValue> write_values{
      {.node_id = std::move(node_id),
       .attribute_id = attribute_id,
       .value = std::move(value),
       .flags = flags}};
  auto inputs = std::make_shared<const std::vector<WriteValue>>(
      std::move(write_values));
  auto executor = co_await boost::asio::this_coro::executor;
  auto statuses = co_await WriteAsync(
      executor, *services.attribute_service, context, inputs);
  if (!statuses.ok()) {
    co_return statuses.status();
  }
  assert(statuses->size() == 1);
  co_return (*statuses)[0];
}

Awaitable<Status> CallNodeAsync(services services,
                                NodeId node_id,
                                ServiceContext context,
                                NodeId method_id,
                                std::vector<Variant> arguments) {
  if (!services.method_service) {
    co_return StatusCode::Bad_Disconnected;
  }

  auto executor = co_await boost::asio::this_coro::executor;
  co_return co_await CallAsync(executor, *services.method_service,
                               std::move(node_id), std::move(method_id),
                               std::move(arguments), context.user_id());
}

Awaitable<StatusOr<DataValue>> ReadNodeAsync(services services,
                                             NodeId node_id,
                                             ServiceContext context,
                                             AttributeId attribute_id) {
  if (!services.attribute_service) {
    co_return StatusCode::Bad_Disconnected;
  }

  std::vector<ReadValueId> read_values{
      {.node_id = std::move(node_id), .attribute_id = attribute_id}};
  auto inputs = std::make_shared<const std::vector<ReadValueId>>(
      std::move(read_values));
  auto executor = co_await boost::asio::this_coro::executor;
  auto results = co_await ReadAsync(
      executor, *services.attribute_service, context, inputs);
  if (!results.ok()) {
    co_return results.status();
  }
  assert(results->size() == 1);
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

Awaitable<StatusOr<DataValue>> node::read(AttributeId attribute_id) const {
  return ReadNodeAsync(services_, node_id_, context_, attribute_id);
}

Awaitable<Status> node::write(AttributeId attribute_id,
                              Variant value,
                              scada::WriteFlags flags) const {
  return WriteNodeAsync(services_, node_id_, context_, attribute_id,
                        std::move(value), flags);
}

Awaitable<StatusOr<std::vector<ReferenceDescription>>> node::browse(
    const browse_details& details) const {
  if (!services_.view_service) {
    co_return StatusCode::Bad_Disconnected;
  }

  auto executor = co_await boost::asio::this_coro::executor;
  std::vector<BrowseDescription> nodes{
      {.node_id = node_id_,
       .direction = details.direction,
       .reference_type_id = details.reference_type_id}};
  auto results = co_await BrowseAsync(executor, *services_.view_service,
                                      context_, nodes);
  if (!results.ok()) {
    co_return results.status();
  }
  assert(results->size() == 1);
  if (!IsGood((*results)[0].status_code)) {
    co_return (*results)[0].status_code;
  }
  co_return std::move((*results)[0].references);
}

Awaitable<StatusOr<scada::node>> node::browse_node(
    const browse_details& details) const {
  auto results = co_await browse(details);
  if (!results.ok()) {
    co_return results.status();
  }
  if (results->size() != 1) {
    co_return StatusCode::Bad;
  }
  co_return scada::node{services_, (*results)[0].node_id, context_};
}

Awaitable<StatusOr<std::vector<BrowsePathTarget>>> node::translate_browse_path(
    const RelativePath& relative_path) const {
  if (!services_.view_service) {
    co_return StatusCode::Bad_Disconnected;
  }

  auto executor = co_await boost::asio::this_coro::executor;
  std::vector<BrowsePath> paths{
      {.node_id = node_id_, .relative_path = relative_path}};
  auto results = co_await TranslateBrowsePathsAsync(
      executor, *services_.view_service, paths);
  if (!results.ok()) {
    co_return results.status();
  }
  assert(results->size() == 1);
  if (!IsGood((*results)[0].status_code)) {
    co_return (*results)[0].status_code;
  }
  co_return std::move((*results)[0].targets);
}

Awaitable<StatusOr<NodeId>> node::child_id(
    scada::QualifiedName browse_name) const {
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

Awaitable<StatusOr<node>> node::child_node(
    scada::QualifiedName browse_name) const {
  auto id = co_await child_id(std::move(browse_name));
  if (!id.ok()) {
    co_return id.status();
  }
  co_return scada::node{services_, *id, context_};
}

Awaitable<Status> node::call_packed(
    NodeId method_id,
    std::vector<Variant> arguments) const {
  return CallNodeAsync(services_, node_id_, context_, std::move(method_id),
                       std::move(arguments));
}

Awaitable<StatusOr<std::vector<scada::DataValue>>> node::read_value_history(
    const HistoryReadRawDetails& details) const {
  assert(details.node_id.is_null());
  assert(details.continuation_point.empty());

  std::vector<scada::DataValue> values;
  auto next_details = details;

  for (;;) {
    auto result = co_await read_value_history_chunk(next_details);
    if (!result.status) {
      co_return result.status;
    }
    values.insert(values.end(), result.values.begin(), result.values.end());
    if (result.continuation_point.empty()) {
      co_return values;
    }
    next_details.continuation_point = result.continuation_point;
  }
}

Awaitable<HistoryReadRawResult> node::read_value_history_chunk(
    const HistoryReadRawDetails& details) const {
  assert(details.node_id.is_null());

  if (!services_.history_service) {
    co_return HistoryReadRawResult{.status = StatusCode::Bad_Disconnected};
  }

  auto sanitized_details = details;
  sanitized_details.node_id = node_id_;
  auto executor = co_await boost::asio::this_coro::executor;
  auto result = co_await HistoryReadRawAsync(
      executor, *services_.history_service, sanitized_details);
  co_return result;
}

Awaitable<StatusOr<std::vector<Event>>> node::read_event_history(
    const event_history_details& details) const {
  if (!services_.history_service) {
    co_return StatusCode::Bad_Disconnected;
  }

  auto executor = co_await boost::asio::this_coro::executor;
  auto result = co_await HistoryReadEventsAsync(
      executor, *services_.history_service, node_id_, details.from,
      details.to, details.filter);
  if (!result.status) {
    co_return result.status;
  }
  co_return std::move(result.events);
}

}  // namespace scada
