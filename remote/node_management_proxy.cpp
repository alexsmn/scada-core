#include "remote/node_management_proxy.h"

#include "base/strings/sys_string_conversions.h"
#include "base/strings/utf_string_conversions.h"
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

void NodeManagementProxy::CreateNode(const scada::NodeId& requested_id,
                                     const scada::NodeId& parent_id,
                                     scada::NodeClass node_class,
                                     const scada::NodeId& type_definition_id,
                                     scada::NodeAttributes attributes,
                                     const CreateNodeCallback& callback) {
  LOG_INFO(logger_) << "CreateNode request"
                    << LOG_TAG("RequestedId", NodeIdToScadaString(requested_id))
                    << LOG_TAG("NodeClass", ToString(node_class))
                    << LOG_TAG("ParentId", NodeIdToScadaString(parent_id))
                    << LOG_TAG("TypeDefinitionId",
                               NodeIdToScadaString(type_definition_id));

  if (!sender_)
    return callback(scada::StatusCode::Bad_Disconnected, {});

  protocol::Request request;
  auto& create_node = *request.mutable_create_node();
  create_node.set_node_class(ConvertTo<protocol::NodeClass>(node_class));
  if (!requested_id.is_null())
    Convert(requested_id, *create_node.mutable_requested_node_id());
  Convert(parent_id, *create_node.mutable_parent_id());
  Convert(type_definition_id, *create_node.mutable_type_definition_id());
  if (!attributes.empty())
    Convert(std::move(attributes), *create_node.mutable_attributes());

  sender_->Request(
      request, [this, callback](const protocol::Response& response) {
        auto status = ConvertTo<scada::Status>(response.status());
        scada::NodeId node_id;
        if (response.has_create_node_result())
          node_id =
              ConvertTo<scada::NodeId>(response.create_node_result().node_id());

        LOG_INFO(logger_) << "CreateNode response"
                          << LOG_TAG("Status", ToString(status))
                          << LOG_TAG("NodeId", NodeIdToScadaString(node_id));
        if (callback)
          callback(std::move(status), node_id);
      });
}

void NodeManagementProxy::DeleteNode(
    const scada::NodeId& node_id,
    bool return_dependencies,
    const scada::DeleteNodeCallback& callback) {
  LOG_INFO(logger_) << "DeleteNode request"
                    << LOG_TAG("NodeId", NodeIdToScadaString(node_id));

  if (!sender_)
    return callback(scada::StatusCode::Bad_Disconnected, {});

  protocol::Request request;
  auto& delete_node = *request.mutable_delete_node();
  Convert(node_id, *delete_node.mutable_node_id());
  if (return_dependencies)
    delete_node.set_return_dependencies(true);

  sender_->Request(request,
                   [this, callback](const protocol::Response& response) {
                     auto status = ConvertTo<scada::Status>(response.status());
                     auto dependencies = ConvertTo<std::vector<scada::NodeId>>(
                         response.delete_node_result().dependencies());

                     LOG_INFO(logger_) << "DeleteNode response"
                                       << LOG_TAG("Status", ToString(status));

                     if (callback)
                       callback(std::move(status), std::move(dependencies));
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

void NodeManagementProxy::AddReference(const scada::NodeId& reference_type_id,
                                       const scada::NodeId& source_id,
                                       const scada::NodeId& target_id,
                                       const scada::StatusCallback& callback) {
  LOG_INFO(logger_) << "AddReference request"
                    << LOG_TAG("ReferenceTypeId",
                               NodeIdToScadaString(reference_type_id))
                    << LOG_TAG("SourceId", NodeIdToScadaString(source_id))
                    << LOG_TAG("TargetId", NodeIdToScadaString(target_id));

  if (!sender_)
    return callback(scada::StatusCode::Bad_Disconnected);

  protocol::Request request;
  auto& add_reference = *request.mutable_add_reference();
  Convert(reference_type_id, *add_reference.mutable_reference_type_id());
  Convert(source_id, *add_reference.mutable_source_id());
  Convert(target_id, *add_reference.mutable_target_id());

  sender_->Request(request,
                   [this, callback](const protocol::Response& response) {
                     auto status = ConvertTo<scada::Status>(response.status());

                     LOG_INFO(logger_) << "AddReference response"
                                       << LOG_TAG("Status", ToString(status));

                     if (callback)
                       callback(std::move(status));
                   });
}

void NodeManagementProxy::DeleteReference(
    const scada::NodeId& reference_type_id,
    const scada::NodeId& source_id,
    const scada::NodeId& target_id,
    const scada::StatusCallback& callback) {
  LOG_INFO(logger_) << "DeleteReference request"
                    << LOG_TAG("ReferenceTypeId",
                               NodeIdToScadaString(reference_type_id))
                    << LOG_TAG("SourceId", NodeIdToScadaString(source_id))
                    << LOG_TAG("TargetId", NodeIdToScadaString(target_id));

  if (!sender_)
    return callback(scada::StatusCode::Bad_Disconnected);

  protocol::Request request;
  auto& delete_reference = *request.mutable_delete_reference();
  Convert(reference_type_id, *delete_reference.mutable_reference_type_id());
  Convert(source_id, *delete_reference.mutable_source_id());
  Convert(target_id, *delete_reference.mutable_target_id());

  sender_->Request(request,
                   [this, callback](const protocol::Response& response) {
                     auto status = ConvertTo<scada::Status>(response.status());

                     LOG_INFO(logger_) << "DeleteReference response"
                                       << LOG_TAG("Status", ToString(status));

                     if (callback)
                       callback(std::move(status));
                   });
}
