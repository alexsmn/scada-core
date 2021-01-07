#include "remote/node_management_proxy.h"

#include "base/logger.h"
#include "base/strings/sys_string_conversions.h"
#include "base/strings/utf_string_conversions.h"
#include "model/node_id_util.h"
#include "remote/message_sender.h"
#include "remote/protocol.h"
#include "remote/protocol_utils.h"

NodeManagementProxy::NodeManagementProxy(std::shared_ptr<Logger> logger)
    : logger_(std::move(logger)) {}

void NodeManagementProxy::OnChannelOpened(MessageSender& sender) {
  sender_ = &sender;
}

void NodeManagementProxy::OnChannelClosed() {
  sender_ = nullptr;
}

void NodeManagementProxy::AddNodes(
    const std::vector<scada::AddNodesItem>& inputs,
    const scada::AddNodesCallback& callback) {
  logger().WriteF(LogSeverity::Normal, "AddNodes request [count=%d]",
                  static_cast<int>(inputs.size()));

  if (!sender_)
    return callback(scada::StatusCode::Bad_Disconnected, {});

  protocol::Request request;
  Convert(inputs, *request.mutable_add_node());

  sender_->Request(request, [this, count = inputs.size(),
                             callback](const protocol::Response& response) {
    auto status = ConvertTo<scada::Status>(response.status());
    auto results = ConvertTo<std::vector<scada::AddNodesResult>>(
        response.add_node_result());

    logger().WriteF(LogSeverity::Normal,
                    "CreateNode response [status=%s, count=%d]",
                    ToString(status).c_str(), static_cast<int>(count));

    if (callback)
      callback(std::move(status), std::move(results));
  });
}

void NodeManagementProxy::DeleteNodes(
    const std::vector<scada::DeleteNodesItem>& inputs,
    const scada::DeleteNodesCallback& callback) {
  logger().WriteF(LogSeverity::Normal, "DeleteNode request [count=%d]",
                  static_cast<int>(inputs.size()));

  if (!sender_)
    return callback(scada::StatusCode::Bad_Disconnected, {});

  protocol::Request request;
  Convert(inputs, *request.mutable_delete_node());

  sender_->Request(request, [this, count = inputs.size(),
                             callback](const protocol::Response& response) {
    auto status = ConvertTo<scada::Status>(response.status());
    auto results = ConvertTo<std::vector<scada::StatusCode>>(
        response.delete_node_result());

    logger().WriteF(LogSeverity::Normal,
                    "DeleteNode response [status=%s, count=%d]",
                    ToString(status).c_str(), static_cast<int>(count));

    if (callback)
      callback(std::move(status), std::move(results));
  });
}

void NodeManagementProxy::ChangeUserPassword(
    const scada::NodeId& user_node_id,
    const scada::LocalizedText& current_password,
    const scada::LocalizedText& new_password,
    const scada::StatusCallback& callback) {
  if (!sender_)
    return callback(scada::StatusCode::Bad_Disconnected);

  protocol::Request request;
  auto& change_password = *request.mutable_change_password();
  Convert(user_node_id, *change_password.mutable_user_node_id());
  change_password.set_current_password_utf8(
      base::UTF16ToUTF8(current_password));
  change_password.set_new_password_utf8(base::UTF16ToUTF8(new_password));

  sender_->Request(request,
                   [this, callback](const protocol::Response& response) {
                     if (callback)
                       callback(ConvertTo<scada::Status>(response.status()));
                   });
}

void NodeManagementProxy::AddReferences(
    const std::vector<scada::AddReferencesItem>& inputs,
    const scada::AddReferencesCallback& callback) {
  logger().WriteF(LogSeverity::Normal, "AddReferences request [count=%d]",
                  inputs.size());

  if (!sender_)
    return callback(scada::StatusCode::Bad_Disconnected, {});

  protocol::Request request;
  Convert(inputs, *request.mutable_add_reference());

  sender_->Request(request,
                   [this, callback](const protocol::Response& response) {
                     if (callback)
                       callback(ConvertTo<scada::Status>(response.status()),
                                ConvertTo<std::vector<scada::StatusCode>>(
                                    response.add_reference_result()));
                   });
}

void NodeManagementProxy::DeleteReferences(
    const std::vector<scada::DeleteReferencesItem>& inputs,
    const scada::DeleteReferencesCallback& callback) {
  logger().WriteF(LogSeverity::Normal, "DeleteReference request [count=%d]",
                  inputs.size());

  if (!sender_)
    return callback(scada::StatusCode::Bad_Disconnected, {});

  protocol::Request request;
  Convert(inputs, *request.mutable_delete_reference());

  sender_->Request(request,
                   [this, callback](const protocol::Response& response) {
                     if (callback)
                       callback(ConvertTo<scada::Status>(response.status()),
                                ConvertTo<std::vector<scada::StatusCode>>(
                                    response.delete_reference_result()));
                   });
}
