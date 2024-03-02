#include "remote/view_service_stub.h"

#include "base/executor.h"
#include "model/node_id_util.h"
#include "model/scada_node_ids.h"
#include "remote/message_sender.h"
#include "remote/protocol.h"
#include "remote/protocol_utils.h"
#include "scada/status.h"
#include "scada/view_service.h"

using namespace std::chrono_literals;

// ViewServiceStub

ViewServiceStub::ViewServiceStub(ViewServiceStubContext&& context)
    : ViewServiceStubContext{std::move(context)} {}

ViewServiceStub::~ViewServiceStub() {}

void ViewServiceStub::OnRequestReceived(const protocol::Request& request) {
  if (request.has_browse()) {
    OnBrowse(request.request_id(), request.browse());
  }

  if (request.browse_path_size() != 0) {
    OnBrowsePaths(request.request_id(), request.browse_path());
  }
}

void ViewServiceStub::OnBrowse(unsigned request_id,
                               const protocol::Browse& browse_request) {
  std::vector<scada::BrowseDescription> inputs;
  inputs.reserve(browse_request.nodes_size());
  for (const protocol::BrowseDescription& proto_node : browse_request.nodes()) {
    inputs.emplace_back(
        ConvertTo<scada::NodeId>(proto_node.node_id()),
        ConvertTo<scada::BrowseDirection>(proto_node.direction()),
        proto_node.has_reference_type_id()
            ? ConvertTo<scada::NodeId>(proto_node.reference_type_id())
            : scada::NodeId{},
        proto_node.include_subtypes());
  }

  service_.Browse(
      service_context_, inputs,
      BindExecutor(executor_,
                   [request_id, sender = sender_](
                       const scada::Status& status,
                       const std::vector<scada::BrowseResult>& results) {
                     protocol::Message message;
                     auto& response = *message.add_responses();
                     response.set_request_id(request_id);
                     Convert(status, *response.mutable_status());
                     auto& browse = *response.mutable_browse_result();
                     if (status)
                       Convert(results, *browse.mutable_results());

                     if (auto locked_sender = sender.lock()) {
                       locked_sender->Send(message);
                     }
                   }));
}

void ViewServiceStub::OnBrowsePaths(
    unsigned request_id,
    const ::google::protobuf::RepeatedPtrField<protocol::BrowsePath>&
        browse_path_requests) {
  auto inputs = ConvertTo<std::vector<scada::BrowsePath>>(browse_path_requests);

  service_.TranslateBrowsePaths(
      inputs,
      BindExecutor(executor_,
                   [request_id, sender = sender_](
                       const scada::Status& status,
                       const std::vector<scada::BrowsePathResult>& results) {
                     protocol::Message message;
                     auto& response = *message.add_responses();
                     response.set_request_id(request_id);
                     Convert(status, *response.mutable_status());
                     Convert(results, *response.mutable_browse_path_result());

                     if (auto locked_sender = sender.lock()) {
                       locked_sender->Send(message);
                     }
                   }));
}
