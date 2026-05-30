#include "scada/node.h"

#include "base/any_executor.h"
#include "base/thread_executor.h"
#include "scada/service_awaitable.h"
#include "scada/service_context.h"
#include "scada/status_awaitable.h"

#include <boost/asio/this_coro.hpp>
#include <memory>

namespace scada {
namespace {

Awaitable<void> WriteNodeAsync(services services,
                               NodeId node_id,
                               ServiceContext context,
                               AttributeId attribute_id,
                               Variant value,
                               scada::WriteFlags flags) {
  if (!services.attribute_service) {
    throw status_exception{StatusCode::Bad};
  }

  auto inputs = std::make_shared<const std::vector<WriteValue>>(
      std::vector<WriteValue>{{.node_id = std::move(node_id),
                               .attribute_id = attribute_id,
                               .value = std::move(value),
                               .flags = flags}});
  auto executor = co_await boost::asio::this_coro::executor;
  auto statuses = ValueOrThrow(co_await WriteAsync(
      executor, *services.attribute_service, context, inputs));
  assert(statuses.size() == 1);
  ThrowIfBad(statuses[0]);
}

Awaitable<void> CallNodeAsync(services services,
                              NodeId node_id,
                              ServiceContext context,
                              NodeId method_id,
                              std::vector<Variant> arguments) {
  if (!services.method_service) {
    throw status_exception{StatusCode::Bad_Disconnected};
  }

  auto executor = co_await boost::asio::this_coro::executor;
  ThrowIfBad(co_await CallAsync(executor, *services.method_service,
                                std::move(node_id), std::move(method_id),
                                std::move(arguments), context.user_id()));
}

}  // namespace

node::node() = default;

node node::with_context(const ServiceContext& context) const {
  return node{services_, node_id_, context};
}

Awaitable<DataValue> node::read(AttributeId attribute_id) const {
  if (!services_.attribute_service) {
    throw status_exception{StatusCode::Bad_Disconnected};
  }

  auto inputs = std::make_shared<const std::vector<ReadValueId>>(
      std::vector<ReadValueId>{{.node_id = node_id_,
                                .attribute_id = attribute_id}});
  auto results = ValueOrThrow(co_await ReadAsync(
      ::ThreadExecutor{}, *services_.attribute_service, context_, inputs));
  assert(results.size() == 1);
  ThrowIfBad(results[0].status_code);
  co_return std::move(results[0]);
}

Awaitable<void> node::write(AttributeId attribute_id,
                            Variant value,
                            scada::WriteFlags flags) const {
  return WriteNodeAsync(services_, node_id_, context_, attribute_id,
                        std::move(value), flags);
}

Awaitable<std::vector<ReferenceDescription>> node::browse(
    const browse_details& details) const {
  if (!services_.view_service) {
    throw status_exception{StatusCode::Bad_Disconnected};
  }

  auto results = ValueOrThrow(co_await BrowseAsync(
      ::ThreadExecutor{}, *services_.view_service, context_,
      {{.node_id = node_id_,
        .direction = details.direction,
        .reference_type_id = details.reference_type_id}}));
  assert(results.size() == 1);
  ThrowIfBad(results[0].status_code);
  co_return std::move(results[0].references);
}

Awaitable<scada::node> node::browse_node(const browse_details& details) const {
  auto results = co_await browse(details);
  if (results.size() != 1) {
    throw status_exception{StatusCode::Bad};
  }
  co_return scada::node{services_, results[0].node_id, context_};
}

Awaitable<std::vector<BrowsePathTarget>> node::translate_browse_path(
    const RelativePath& relative_path) const {
  if (!services_.view_service) {
    throw status_exception{StatusCode::Bad_Disconnected};
  }

  auto results = ValueOrThrow(co_await TranslateBrowsePathsAsync(
      ::ThreadExecutor{}, *services_.view_service,
      {{.node_id = node_id_, .relative_path = relative_path}}));
  assert(results.size() == 1);
  ThrowIfBad(results[0].status_code);
  co_return std::move(results[0].targets);
}

Awaitable<NodeId> node::child_id(scada::QualifiedName browse_name) const {
  auto targets =
      co_await translate_browse_path({{.reference_type_id = id::HasChild,
                                       .target_name = std::move(browse_name)}});
  if (targets.size() != 1) {
    throw status_exception{StatusCode::Bad_BrowseNameInvalid};
  }
  co_return targets[0].target_id.node_id();
}

Awaitable<node> node::child_node(scada::QualifiedName browse_name) const {
  co_return scada::node{services_, co_await child_id(std::move(browse_name)),
                        context_};
}

Awaitable<void> node::call_packed(
    NodeId method_id,
    std::vector<Variant> arguments) const {
  return CallNodeAsync(services_, node_id_, context_, std::move(method_id),
                       std::move(arguments));
}

Awaitable<std::vector<scada::DataValue>> node::read_value_history(
    const HistoryReadRawDetails& details) const {
  assert(details.node_id.is_null());
  assert(details.continuation_point.empty());

  std::vector<scada::DataValue> values;
  auto next_details = details;

  for (;;) {
    auto result = co_await read_value_history_chunk(next_details);
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
    throw status_exception{StatusCode::Bad_Disconnected};
  }

  auto sanitized_details = details;
  sanitized_details.node_id = node_id_;
  auto result = co_await HistoryReadRawAsync(
      ::ThreadExecutor{}, *services_.history_service, sanitized_details);
  ThrowIfBad(result.status);
  co_return result;
}

Awaitable<std::vector<Event>> node::read_event_history(
    const event_history_details& details) const {
  if (!services_.history_service) {
    throw status_exception{StatusCode::Bad_Disconnected};
  }

  auto result = co_await HistoryReadEventsAsync(
      ::ThreadExecutor{}, *services_.history_service, node_id_, details.from,
      details.to, details.filter);
  ThrowIfBad(result.status);
  co_return std::move(result.events);
}

}  // namespace scada
