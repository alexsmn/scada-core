#include "scada/client.h"

#include "base/any_executor.h"
#include "scada/node_management_service.h"
#include "scada/service_awaitable.h"
#include "scada/service_context.h"

#include <boost/asio/this_coro.hpp>

namespace scada {

client::client() = default;

client::client(const services& services) : services_{services} {}

client client::with_context(const ServiceContext& context) const {
  return client{services_, context};
}

Awaitable<Status> client::connect(SessionConnectParams params) const {
  if (!services_.session_service) {
    co_return StatusCode::Bad_Disconnected;
  }

  co_await services_.session_service->Connect(std::move(params));
  co_return OkStatus();
}

Awaitable<Status> client::disconnect() const {
  if (!services_.session_service) {
    co_return StatusCode::Bad_Disconnected;
  }

  co_await services_.session_service->Disconnect();
  co_return OkStatus();
}

Awaitable<StatusOr<
    std::vector<scada::StatusOr<std::vector<scada::ReferenceDescription>>>>>
client::browse(const std::vector<scada::BrowseDescription>& inputs) const {
  if (!services_.view_service) {
    co_return StatusCode::Bad_Disconnected;
  }

  auto executor = co_await boost::asio::this_coro::executor;
  auto results =
      co_await BrowseAsync(executor, *services_.view_service, context_, inputs);
  if (!results.ok()) {
    co_return results.status();
  }

  std::vector<scada::StatusOr<std::vector<scada::ReferenceDescription>>> output;
  output.reserve(results->size());
  for (auto& result : *results) {
    output.emplace_back(IsGood(result.status_code)
                            ? StatusOr<std::vector<ReferenceDescription>>{
                                  std::move(result.references)}
                            : StatusOr<std::vector<ReferenceDescription>>{
                                  result.status_code});
  }
  co_return output;
}

Awaitable<StatusOr<scada::node>> client::add_node(AddNodesItem item) const {
  if (!services_.node_management_service) {
    co_return StatusCode::Bad_Disconnected;
  }

  auto executor = co_await boost::asio::this_coro::executor;
  auto results = co_await AddNodesAsync(
      executor, *services_.node_management_service,
      {std::move(item)});
  if (!results.ok()) {
    co_return results.status();
  }
  assert(results->size() == 1);
  const auto& [status_code, node_id] = (*results)[0];
  if (!IsGood(status_code)) {
    co_return status_code;
  }
  assert(!node_id.is_null());
  co_return node(node_id);
}

Awaitable<Status> client::acknowledge_events(std::vector<EventId> event_ids,
                                             DateTime acknowledge_time) const {
  assert(!event_ids.empty());
  co_return co_await server_node().call(
      scada::id::AcknowledgeableConditionType_Acknowledge,
      std::move(event_ids), acknowledge_time);
}

}  // namespace scada
