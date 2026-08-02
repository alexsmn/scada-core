#include "remote/node_management_proxy.h"

#include "base/callback_awaitable.h"
#include "remote/message_sender.h"
#include "remote/protocol.h"
#include "remote/protocol_utils.h"
#include "scada/co_result.h"

void NodeManagementProxy::OnChannelOpened(MessageSender& sender) {
  sender_ = &sender;
}

void NodeManagementProxy::OnChannelClosed() {
  sender_ = nullptr;
}

scada::CoStatusOr<std::vector<scada::AddNodesResult>>
NodeManagementProxy::AddNodes(scada::ServiceContext /*context*/,
                              std::vector<scada::AddNodesItem> inputs) {
  LOG_INFO(logger_) << "AddNodes request" << LOG_TAG("Count", inputs.size());

  if (!sender_) {
    LOG_WARNING(logger_) << "AddNodes failed: disconnected";
    co_return scada::Status{scada::StatusCode::Bad_Disconnected};
  }

  protocol::Request request;
  Convert(inputs, *request.mutable_add_node());

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
  auto results =
      ConvertTo<std::vector<scada::AddNodesResult>>(response.add_node_result());

  LOG_INFO(logger_) << "AddNodes response"
                    << LOG_TAG("Status", ToString(status))
                    << LOG_TAG("Count", inputs.size());
  co_return status
      ? scada::StatusOr<std::vector<scada::AddNodesResult>>{std::move(results)}
      : scada::StatusOr<std::vector<scada::AddNodesResult>>{std::move(status)};
}

scada::CoStatusOr<std::vector<scada::StatusCode>>
NodeManagementProxy::DeleteNodes(scada::ServiceContext /*context*/,
                                 std::vector<scada::DeleteNodesItem> inputs) {
  LOG_INFO(logger_) << "DeleteNodes request" << LOG_TAG("Count", inputs.size());

  if (!sender_) {
    LOG_WARNING(logger_) << "DeleteNodes failed: disconnected";
    co_return scada::Status{scada::StatusCode::Bad_Disconnected};
  }

  protocol::Request request;
  Convert(inputs, *request.mutable_delete_node());

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
  auto results =
      ConvertTo<std::vector<scada::StatusCode>>(response.delete_node_result());

  LOG_INFO(logger_) << "DeleteNodes response"
                    << LOG_TAG("Status", ToString(status))
                    << LOG_TAG("Count", inputs.size());
  co_return status
      ? scada::StatusOr<std::vector<scada::StatusCode>>{std::move(results)}
      : scada::StatusOr<std::vector<scada::StatusCode>>{std::move(status)};
}

scada::CoStatusOr<std::vector<scada::StatusCode>>
NodeManagementProxy::AddReferences(
    scada::ServiceContext /*context*/,
    std::vector<scada::AddReferencesItem> inputs) {
  LOG_INFO(logger_) << "AddReferences request"
                    << LOG_TAG("Count", inputs.size());

  if (!sender_) {
    LOG_WARNING(logger_) << "AddReferences failed: disconnected";
    co_return scada::Status{scada::StatusCode::Bad_Disconnected};
  }

  protocol::Request request;
  Convert(inputs, *request.mutable_add_reference());

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
  auto results = ConvertTo<std::vector<scada::StatusCode>>(
      response.add_reference_result());

  LOG_INFO(logger_) << "AddReferences response"
                    << LOG_TAG("Count", inputs.size())
                    << LOG_TAG("Status", ToString(status));
  co_return status
      ? scada::StatusOr<std::vector<scada::StatusCode>>{std::move(results)}
      : scada::StatusOr<std::vector<scada::StatusCode>>{std::move(status)};
}

scada::CoStatusOr<std::vector<scada::StatusCode>>
NodeManagementProxy::DeleteReferences(
    scada::ServiceContext /*context*/,
    std::vector<scada::DeleteReferencesItem> inputs) {
  LOG_INFO(logger_) << "DeleteReferences request"
                    << LOG_TAG("Count", inputs.size());

  if (!sender_) {
    LOG_WARNING(logger_) << "DeleteReferences failed: disconnected";
    co_return scada::Status{scada::StatusCode::Bad_Disconnected};
  }

  protocol::Request request;
  Convert(inputs, *request.mutable_delete_reference());

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
  auto results = ConvertTo<std::vector<scada::StatusCode>>(
      response.delete_reference_result());

  LOG_INFO(logger_) << "DeleteReferences response"
                    << LOG_TAG("Count", inputs.size())
                    << LOG_TAG("Status", ToString(status));
  co_return status
      ? scada::StatusOr<std::vector<scada::StatusCode>>{std::move(results)}
      : scada::StatusOr<std::vector<scada::StatusCode>>{std::move(status)};
}
