#include "remote/node_management_proxy.h"

#include "base/strings/sys_string_conversions.h"
#include "model/node_id_util.h"
#include "remote/message_sender.h"
#include "remote/protocol.h"
#include "remote/protocol_utils.h"

void NodeManagementProxy::OnChannelOpened(MessageSender& sender) {
  sender_ = &sender;
}

void NodeManagementProxy::OnChannelClosed() {
  sender_ = nullptr;
}

void NodeManagementProxy::AddNodes(
    const std::vector<scada::AddNodesItem>& inputs,
    const scada::AddNodesCallback& callback) {
  LOG_INFO(logger_) << "AddNodes request" << LOG_TAG("Count", inputs.size());

  if (!sender_) {
    LOG_WARNING(logger_) << "AddNodes failed: disconnected";
    return callback(scada::StatusCode::Bad_Disconnected, {});
  }

  protocol::Request request;
  Convert(inputs, *request.mutable_add_node());

  sender_->Request(request, [this, count = inputs.size(),
                             callback](const protocol::Response& response) {
    auto status = ConvertTo<scada::Status>(response.status());
    auto results = ConvertTo<std::vector<scada::AddNodesResult>>(
        response.add_node_result());

    LOG_INFO(logger_) << "AddNodes response"
                      << LOG_TAG("Status", ToString(status))
                      << LOG_TAG("Count", count);

    if (callback)
      callback(std::move(status), std::move(results));
  });
}

void NodeManagementProxy::DeleteNodes(
    const std::vector<scada::DeleteNodesItem>& inputs,
    const scada::DeleteNodesCallback& callback) {
  LOG_INFO(logger_) << "DeleteNodes request" << LOG_TAG("Count", inputs.size());

  if (!sender_) {
    LOG_WARNING(logger_) << "DeleteNodes failed: disconnected";
    return callback(scada::StatusCode::Bad_Disconnected, {});
  }

  protocol::Request request;
  Convert(inputs, *request.mutable_delete_node());

  sender_->Request(request, [this, count = inputs.size(),
                             callback](const protocol::Response& response) {
    auto status = ConvertTo<scada::Status>(response.status());
    auto results = ConvertTo<std::vector<scada::StatusCode>>(
        response.delete_node_result());

    LOG_INFO(logger_) << "DeleteNodes response"
                      << LOG_TAG("Status", ToString(status))
                      << LOG_TAG("Count", count);

    if (callback)
      callback(std::move(status), std::move(results));
  });
}

void NodeManagementProxy::AddReferences(
    const std::vector<scada::AddReferencesItem>& inputs,
    const scada::AddReferencesCallback& callback) {
  LOG_INFO(logger_) << "AddReferences request"
                    << LOG_TAG("Count", inputs.size());

  if (!sender_) {
    LOG_WARNING(logger_) << "AddReferences failed: disconnected";
    return callback(scada::StatusCode::Bad_Disconnected, {});
  }

  protocol::Request request;
  Convert(inputs, *request.mutable_add_reference());

  sender_->Request(request, [this, callback, count = inputs.size()](
                                const protocol::Response& response) {
    auto status = ConvertTo<scada::Status>(response.status());
    auto results = ConvertTo<std::vector<scada::StatusCode>>(
        response.add_reference_result());

    LOG_INFO(logger_) << "AddReferences response" << LOG_TAG("Count", count)
                      << LOG_TAG("Status", ToString(status));

    if (callback)
      callback(std::move(status), std::move(results));
  });
}

void NodeManagementProxy::DeleteReferences(
    const std::vector<scada::DeleteReferencesItem>& inputs,
    const scada::DeleteReferencesCallback& callback) {
  LOG_INFO(logger_) << "DeleteReferences request"
                    << LOG_TAG("Count", inputs.size());

  if (!sender_) {
    LOG_WARNING(logger_) << "DeleteReferences failed: disconnected";
    return callback(scada::StatusCode::Bad_Disconnected, {});
  }

  protocol::Request request;
  Convert(inputs, *request.mutable_delete_reference());

  sender_->Request(request, [this, callback, count = inputs.size()](
                                const protocol::Response& response) {
    auto status = ConvertTo<scada::Status>(response.status());
    auto results = ConvertTo<std::vector<scada::StatusCode>>(
        response.delete_reference_result());

    LOG_INFO(logger_) << "DeleteReferences response" << LOG_TAG("Count", count)
                      << LOG_TAG("Status", ToString(status));

    if (callback)
      callback(std::move(status), std::move(results));
  });
}
