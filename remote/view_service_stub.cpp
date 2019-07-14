#include "remote/view_service_stub.h"

#include "core/status.h"
#include "core/view_service.h"
#include "remote/message_sender.h"
#include "remote/protocol.h"
#include "remote/protocol_utils.h"

using namespace std::chrono_literals;

namespace {
const auto kEventConsolidationDelay = 100ms;
}

// ViewServiceStub

ViewServiceStub::ViewServiceStub(ViewServiceStubContext&& context)
    : ViewServiceStubContext{std::move(context)},
      timer_{io_context_},
      weak_factory_(this) {}

ViewServiceStub::~ViewServiceStub() {}

void ViewServiceStub::OnRequestReceived(const protocol::Request& request) {
  if (request.has_browse()) {
    auto& proto_nodes = request.browse().nodes();
    std::vector<scada::BrowseDescription> nodes;
    nodes.reserve(proto_nodes.size());
    for (auto& proto_node : proto_nodes) {
      nodes.push_back({
          FromProto(proto_node.node_id()),
          proto_node.has_direction() ? FromProto(proto_node.direction())
                                     : scada::BrowseDirection::Both,
          proto_node.has_reference_type_id()
              ? FromProto(proto_node.reference_type_id())
              : scada::NodeId{},
          proto_node.include_subtypes(),
      });
    }
    OnBrowse(request.request_id(), std::move(nodes));
  }
}

void ViewServiceStub::OnBrowse(
    unsigned request_id,
    const std::vector<scada::BrowseDescription>& nodes) {
  auto weak_ptr = weak_factory_.GetWeakPtr();
  service_.Browse(nodes, [=](const scada::Status& status,
                             std::vector<scada::BrowseResult> results) {
    if (!weak_ptr)
      return;

    protocol::Message message;
    auto& response = *message.add_responses();
    response.set_request_id(request_id);
    ToProto(status, *response.mutable_status());
    auto& browse = *response.mutable_browse_result();
    if (status)
      ToProto(results, *browse.mutable_results());

    sender_.Send(message);
  });
}
