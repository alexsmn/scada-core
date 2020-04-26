#include "remote/view_service_proxy.h"

#include "base/logger.h"
#include "core/standard_node_ids.h"
#include "core/status.h"
#include "model/node_id_util.h"
#include "remote/message_sender.h"
#include "remote/protocol.h"
#include "remote/protocol_utils.h"

// ViewServiceProxy

ViewServiceProxy::ViewServiceProxy(std::shared_ptr<Logger> logger)
    : logger_(std::move(logger)), sender_(nullptr) {}

void ViewServiceProxy::OnChannelOpened(MessageSender& sender) {
  sender_ = &sender;
}

void ViewServiceProxy::OnChannelClosed() {
  sender_ = nullptr;
}

void ViewServiceProxy::Browse(
    const std::vector<scada::BrowseDescription>& nodes,
    const scada::BrowseCallback& callback) {
  if (!sender_)
    return callback(scada::StatusCode::Bad_Disconnected, {});

  protocol::Request request;
  auto& browse = *request.mutable_browse();
  for (auto& node : nodes) {
    auto& browse_node = *browse.add_nodes();
    ToProto(node.node_id, *browse_node.mutable_node_id());
    if (node.direction != scada::BrowseDirection::Both)
      browse_node.set_direction(ToProto(node.direction));
    ToProto(node.reference_type_id, *browse_node.mutable_reference_type_id());
    if (node.include_subtypes)
      browse_node.set_include_subtypes(true);
  }

  sender_->Request(request, [callback](const protocol::Response& response) {
    callback(FromProto(response.status()),
             VectorFromProto<scada::BrowseResult>(
                 response.browse_result().results()));
  });
}

void ViewServiceProxy::TranslateBrowsePaths(
    const std::vector<scada::BrowsePath>& browse_paths,
    const scada::TranslateBrowsePathCallback& callback) {
  callback(scada::StatusCode::Bad, {});
}
