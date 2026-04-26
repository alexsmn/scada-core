#include "remote/view_service_stub.h"

#include "base/executor.h"
#include "metrics/tracer.h"
#include "model/node_id_util.h"
#include "model/scada_node_ids.h"
#include "remote/message_sender.h"
#include "remote/protocol.h"
#include "remote/protocol_utils.h"
#include "scada/coroutine_services.h"
#include "scada/status.h"
#include "scada/view_service.h"

using namespace std::chrono_literals;

// ViewServiceStub

ViewServiceStub::ViewServiceStub(ViewServiceStubContext&& context)
    : ViewServiceStubContext{std::move(context)} {}

ViewServiceStub::~ViewServiceStub() {}

void ViewServiceStub::OnRequestReceived(const protocol::Request& request) {
  if (request.has_browse()) {
    OnBrowse(request);
  }

  if (request.browse_path_size() != 0) {
    OnBrowsePaths(request);
  }
}

void ViewServiceStub::OnBrowse(const protocol::Request& request) {
  std::vector<scada::BrowseDescription> inputs;
  inputs.reserve(request.browse().nodes_size());
  for (const protocol::BrowseDescription& proto_node :
       request.browse().nodes()) {
    inputs.emplace_back(
        ConvertTo<scada::NodeId>(proto_node.node_id()),
        ConvertTo<scada::BrowseDirection>(proto_node.direction()),
        proto_node.has_reference_type_id()
            ? ConvertTo<scada::NodeId>(proto_node.reference_type_id())
            : scada::NodeId{},
        proto_node.include_subtypes());
  }

  scada::ServiceContext context =
      service_context_.with_request_id(request.request_id())
          .with_trace_id(request.trace_id());

  auto self = shared_from_this();
  CoSpawn(
      executor_,
      [self, request_id = request.request_id(), context = std::move(context),
       inputs = std::move(inputs)]() mutable -> Awaitable<void> {
        co_await self->OnBrowseAsync(request_id, std::move(context),
                                     std::move(inputs));
      });
}

void ViewServiceStub::OnBrowsePaths(const protocol::Request& request) {
  auto inputs =
      ConvertTo<std::vector<scada::BrowsePath>>(request.browse_path());

  auto self = shared_from_this();
  CoSpawn(
      executor_,
      [self, request_id = request.request_id(),
       inputs = std::move(inputs)]() mutable -> Awaitable<void> {
        co_await self->OnBrowsePathsAsync(request_id, std::move(inputs));
      });
}

Awaitable<void> ViewServiceStub::OnBrowseAsync(
    unsigned request_id,
    scada::ServiceContext context,
    std::vector<scada::BrowseDescription> inputs) {
  auto result =
      co_await coroutine_service_.Browse(std::move(context), std::move(inputs));
  auto status = result.status();
  auto results = std::move(result).value_or({});

  LOG_INFO(logger_) << "Browse async completed"
                    << LOG_TAG("RequestId", request_id)
                    << LOG_TAG("Status", ToString(status))
                    << LOG_TAG("ResultCount", results.size());

  protocol::Message message;
  auto& response = *message.add_responses();
  response.set_request_id(request_id);
  Convert(status, *response.mutable_status());
  if (status) {
    Convert(std::move(results),
            *response.mutable_browse_result()->mutable_results());
  }

  if (auto locked_sender = sender_.lock()) {
    locked_sender->Send(message);
  }
}

Awaitable<void> ViewServiceStub::OnBrowsePathsAsync(
    unsigned request_id,
    std::vector<scada::BrowsePath> inputs) {
  auto result =
      co_await coroutine_service_.TranslateBrowsePaths(std::move(inputs));
  auto status = result.status();
  auto results = std::move(result).value_or({});

  protocol::Message message;
  auto& response = *message.add_responses();
  response.set_request_id(request_id);
  Convert(status, *response.mutable_status());
  Convert(std::move(results), *response.mutable_browse_path_result());

  if (auto locked_sender = sender_.lock()) {
    locked_sender->Send(message);
  }
}
