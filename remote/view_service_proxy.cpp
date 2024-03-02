#include "remote/view_service_proxy.h"

#include "remote/message_sender.h"
#include "remote/protocol.h"
#include "remote/protocol_utils.h"
#include "scada/service_context.h"

// ViewServiceProxy

void ViewServiceProxy::OnChannelOpened(MessageSender& sender) {
  sender_ = &sender;
}

void ViewServiceProxy::OnChannelClosed() {
  sender_ = nullptr;
}

void ViewServiceProxy::Browse(
    const scada::ServiceContext& context,
    const std::vector<scada::BrowseDescription>& nodes,
    const scada::BrowseCallback& callback) {
  if (!sender_)
    return callback(scada::StatusCode::Bad_Disconnected, {});

  protocol::Request request;
  request.set_trace_id(context.trace_id());
  auto& browse = *request.mutable_browse();
  for (auto& node : nodes) {
    auto& browse_node = *browse.add_nodes();
    Convert(node.node_id, *browse_node.mutable_node_id());
    browse_node.set_direction(
        ConvertTo<protocol::BrowseDirection>(node.direction));
    Convert(node.reference_type_id, *browse_node.mutable_reference_type_id());
    if (node.include_subtypes)
      browse_node.set_include_subtypes(true);
  }

  sender_->Request(request, [callback](const protocol::Response& response) {
    callback(ConvertTo<scada::Status>(response.status()),
             ConvertTo<std::vector<scada::BrowseResult>>(
                 response.browse_result().results()));
  });
}

void ViewServiceProxy::TranslateBrowsePaths(
    const std::vector<scada::BrowsePath>& browse_paths,
    const scada::TranslateBrowsePathsCallback& callback) {
  if (!sender_)
    return callback(scada::StatusCode::Bad_Disconnected, {});

  protocol::Request request;
  Convert(browse_paths, *request.mutable_browse_path());

  sender_->Request(request, [callback](const protocol::Response& response) {
    callback(ConvertTo<scada::Status>(response.status()),
             ConvertTo<std::vector<scada::BrowsePathResult>>(
                 response.browse_path_result()));
  });
}
