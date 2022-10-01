#include "remote/view_service_stub.h"

#include "base/executor.h"
#include "core/status.h"
#include "core/view_service.h"
#include "model/node_id_util.h"
#include "model/scada_node_ids.h"
#include "remote/message_sender.h"
#include "remote/protocol.h"
#include "remote/protocol_utils.h"

using namespace std::chrono_literals;

// ViewServiceStub

ViewServiceStub::ViewServiceStub(ViewServiceStubContext&& context)
    : ViewServiceStubContext{std::move(context)} {}

ViewServiceStub::~ViewServiceStub() {}

void ViewServiceStub::OnRequestReceived(const protocol::Request& request) {
  if (request.has_browse()) {
    auto& proto_nodes = request.browse().nodes();
    std::vector<scada::BrowseDescription> nodes;
    nodes.reserve(proto_nodes.size());
    for (auto& proto_node : proto_nodes) {
      nodes.push_back({
          ConvertTo<scada::NodeId>(proto_node.node_id()),
          ConvertTo<scada::BrowseDirection>(proto_node.direction()),
          proto_node.has_reference_type_id()
              ? ConvertTo<scada::NodeId>(proto_node.reference_type_id())
              : scada::NodeId{},
          proto_node.include_subtypes(),
      });
    }
    OnBrowse(request.request_id(), std::move(nodes));
  }

  if (request.browse_path_size() != 0)
    OnBrowsePaths(request);
}

void ViewServiceStub::OnBrowse(
    unsigned request_id,
    const std::vector<scada::BrowseDescription>& nodes) {
  service_.Browse(
      nodes,
      BindExecutor(executor_, [request_id, sender = sender_](
                                  const scada::Status& status,
                                  std::vector<scada::BrowseResult> results) {
        protocol::Message message;
        auto& response = *message.add_responses();
        response.set_request_id(request_id);
        Convert(status, *response.mutable_status());
        auto& browse = *response.mutable_browse_result();
        if (status)
          Convert(results, *browse.mutable_results());

        if (auto locked_sender = sender.lock())
          locked_sender->Send(message);
      }));
}

void ViewServiceStub::OnBrowsePaths(const protocol::Request& request) {
  auto inputs =
      ConvertTo<std::vector<scada::BrowsePath>>(request.browse_path());

  service_.TranslateBrowsePaths(
      inputs,
      BindExecutor(executor_,
                   [request_id = request.request_id(), sender = sender_](
                       const scada::Status& status,
                       std::vector<scada::BrowsePathResult> results) {
                     protocol::Message message;
                     auto& response = *message.add_responses();
                     response.set_request_id(request_id);
                     Convert(status, *response.mutable_status());
                     Convert(results, *response.mutable_browse_path_result());

                     if (auto locked_sender = sender.lock())
                       locked_sender->Send(message);
                   }));
}
