#include "remote/view_service_proxy.h"

#include "base/callback_awaitable.h"
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

Awaitable<scada::StatusOr<std::vector<scada::BrowseResult>>>
ViewServiceProxy::Browse(scada::ServiceContext context,
                         std::vector<scada::BrowseDescription> nodes) {
  if (!sender_)
    co_return scada::Status{scada::StatusCode::Bad_Disconnected};

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

  auto executor = co_await boost::asio::this_coro::executor;
  auto [response] = co_await CallbackToAwaitable<protocol::Response>(
      executor, [this, &request](auto callback) mutable {
        sender_->Request(request,
                         [callback = std::move(callback)](
                             const protocol::Response& response) mutable {
                           callback(response);
                         });
      });

  auto status = ConvertTo<scada::Status>(response.status());
  auto results = ConvertTo<std::vector<scada::BrowseResult>>(
      response.browse_result().results());
  co_return status
                ? scada::StatusOr<std::vector<scada::BrowseResult>>{
                      std::move(results)}
                : scada::StatusOr<std::vector<scada::BrowseResult>>{
                      std::move(status)};
}

Awaitable<scada::StatusOr<std::vector<scada::BrowsePathResult>>>
ViewServiceProxy::TranslateBrowsePaths(
    std::vector<scada::BrowsePath> browse_paths) {
  if (!sender_)
    co_return scada::Status{scada::StatusCode::Bad_Disconnected};

  protocol::Request request;
  Convert(browse_paths, *request.mutable_browse_path());

  auto executor = co_await boost::asio::this_coro::executor;
  auto [response] = co_await CallbackToAwaitable<protocol::Response>(
      executor, [this, &request](auto callback) mutable {
        sender_->Request(request,
                         [callback = std::move(callback)](
                             const protocol::Response& response) mutable {
                           callback(response);
                         });
      });

  auto status = ConvertTo<scada::Status>(response.status());
  auto results = ConvertTo<std::vector<scada::BrowsePathResult>>(
      response.browse_path_result());
  co_return status
                ? scada::StatusOr<std::vector<scada::BrowsePathResult>>{
                      std::move(results)}
                : scada::StatusOr<std::vector<scada::BrowsePathResult>>{
                      std::move(status)};
}
