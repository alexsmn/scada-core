#include "remote/node_management_stub.h"

#include "base/strings/sys_string_conversions.h"
#include "base/strings/utf_string_conversions.h"
#include "core/node_management_service.h"
#include "remote/message_sender.h"
#include "remote/protocol.h"
#include "remote/protocol_utils.h"

NodeManagementStub::NodeManagementStub(MessageSender& sender,
                                       scada::NodeManagementService& service,
                                       scada::NodeId user_id)
    : sender_(sender), service_(service), user_id_(std::move(user_id)) {}

void NodeManagementStub::OnRequestReceived(const protocol::Request& request) {
  if (request.has_create_node()) {
    auto& create_node = request.create_node();
    OnCreateNode(
        request.request_id(),
        create_node.has_requested_node_id()
            ? ConvertTo<scada::NodeId>(create_node.requested_node_id())
            : scada::NodeId(),
        ConvertTo<scada::NodeId>(create_node.parent_id()),
        ConvertTo<scada::NodeClass>(create_node.node_class()),
        ConvertTo<scada::NodeId>(create_node.type_definition_id()),
        create_node.has_attributes()
            ? ConvertTo<scada::NodeAttributes>(create_node.attributes())
            : scada::NodeAttributes());
  }

  if (request.has_delete_node()) {
    auto& delete_node = request.delete_node();
    OnDeleteNode(request.request_id(),
                 ConvertTo<scada::NodeId>(delete_node.node_id()),
                 delete_node.return_dependencies());
  }

  if (request.has_change_password()) {
    auto& change_password = request.change_password();
    OnChangeUserPassword(
        request.request_id(),
        ConvertTo<scada::NodeId>(change_password.user_node_id()),
        ConvertTo<scada::LocalizedText>(
            change_password.current_password_utf8()),
        ConvertTo<scada::LocalizedText>(change_password.new_password_utf8()));
  }

  if (request.has_add_reference())
    OnAddReference(request.request_id(), request.add_reference());

  if (request.has_delete_reference())
    OnDeleteReference(request.request_id(), request.delete_reference());
}

void NodeManagementStub::OnDeleteNode(unsigned request_id,
                                      const scada::NodeId& id,
                                      bool return_dependencies) {
  if (id == user_id_) {
    protocol::Message message;
    auto& response = *message.add_responses();
    response.set_request_id(request_id);
    auto& delete_node_result = *response.mutable_delete_node_result();
    Convert(scada::StatusCode::Bad_CantDeleteOwnUser,
            *response.mutable_status());
    sender_.Send(message);
    return;
  }

  auto weak_ptr = weak_factory_.GetWeakPtr();
  service_.DeleteNode(
      id, return_dependencies,
      [weak_ptr, request_id](scada::Status&& status,
                             std::vector<scada::NodeId>&& dependencies) {
        auto ptr = weak_ptr.get();
        if (!ptr)
          return;

        protocol::Message message;
        auto& response = *message.add_responses();
        response.set_request_id(request_id);
        auto& delete_node_result = *response.mutable_delete_node_result();
        Convert(status, *response.mutable_status());
        if (!dependencies.empty())
          Convert(dependencies, *delete_node_result.mutable_dependencies());
        ptr->sender_.Send(message);
      });
}

void NodeManagementStub::OnCreateNode(unsigned request_id,
                                      const scada::NodeId& requested_id,
                                      const scada::NodeId& parent_id,
                                      scada::NodeClass node_class,
                                      const scada::NodeId& type_id,
                                      scada::NodeAttributes attributes) {
  LOG_INFO(logger_) << "Create node" << LOG_TAG("RequestId", request_id)
                    << LOG_TAG("RequestedId", ToString(requested_id))
                    << LOG_TAG("ParentId", parent_id)
                    << LOG_TAG("NodeClass", ToString(node_class))
                    << LOG_TAG("TypeDefinitionId", type_id);

  auto weak_ptr = weak_factory_.GetWeakPtr();
  service_.CreateNode(
      requested_id, parent_id, node_class, type_id, std::move(attributes),
      [this, weak_ptr, request_id](const scada::Status& status,
                                   const scada::NodeId& node_id) {
        auto ptr = weak_ptr.get();
        if (!ptr)
          return;

        LOG_INFO(logger_) << "Create node result"
                          << LOG_TAG("RequestId", request_id)
                          << LOG_TAG("Status", ToString(status))
                          << LOG_TAG("NodeId", ToString(node_id));

        protocol::Message message;
        auto& response = *message.add_responses();
        response.set_request_id(request_id);
        auto& create_node_result = *response.mutable_create_node_result();
        Convert(status, *response.mutable_status());
        Convert(node_id, *create_node_result.mutable_node_id());
        sender_.Send(message);
      });
}

void NodeManagementStub::OnChangeUserPassword(
    unsigned request_id,
    const scada::NodeId& user_id,
    const scada::LocalizedText& current_password,
    const scada::LocalizedText& new_password) {
  LOG_INFO(logger_) << "Change user password"
                    << LOG_TAG("RequestId", request_id)
                    << LOG_TAG("UserId", ToString(user_id));

  auto weak_ptr = weak_factory_.GetWeakPtr();
  service_.ChangeUserPassword(
      user_id, current_password, new_password,
      [this, weak_ptr, request_id](const scada::Status& status) {
        auto ptr = weak_ptr.get();
        if (!ptr)
          return;

        LOG_INFO(logger_) << "Change user password"
                          << LOG_TAG("RequestId", request_id)
                          << LOG_TAG("Status", ToString(status));

        protocol::Message message;
        auto& response = *message.add_responses();
        response.set_request_id(request_id);
        Convert(status, *response.mutable_status());
        sender_.Send(message);
      });
}

void NodeManagementStub::OnAddReference(unsigned request_id,
                                        const protocol::Reference& request) {
  auto reference_type_id =
      ConvertTo<scada::NodeId>(request.reference_type_id());
  auto source_id = ConvertTo<scada::NodeId>(request.source_id());
  auto target_id = ConvertTo<scada::NodeId>(request.target_id());

  LOG_INFO(logger_) << "Add reference" << LOG_TAG("RequestId", request_id)
                    << LOG_TAG("ReferenceTypeId", ToString(reference_type_id))
                    << LOG_TAG("SourceId", ToString(source_id))
                    << LOG_TAG("TargetId", ToString(target_id));

  auto weak_ptr = weak_factory_.GetWeakPtr();
  service_.AddReference(
      reference_type_id, source_id, target_id,
      [this, weak_ptr, request_id](const scada::Status& status) {
        auto ptr = weak_ptr.get();
        if (!ptr)
          return;

        LOG_INFO(logger_) << "Add reference" << LOG_TAG("RequestId", request_id)
                          << LOG_TAG("Status", ToString(status));

        protocol::Message message;
        auto& response = *message.add_responses();
        response.set_request_id(request_id);
        Convert(status, *response.mutable_status());
        sender_.Send(message);
      });
}

void NodeManagementStub::OnDeleteReference(unsigned request_id,
                                           const protocol::Reference& request) {
  auto reference_type_id =
      ConvertTo<scada::NodeId>(request.reference_type_id());
  auto source_id = ConvertTo<scada::NodeId>(request.source_id());
  auto target_id = ConvertTo<scada::NodeId>(request.target_id());

  LOG_INFO(logger_) << "Delete reference" << LOG_TAG("RequestId", request_id)
                    << LOG_TAG("ReferenceTypeId", ToString(reference_type_id))
                    << LOG_TAG("SourceId", ToString(source_id))
                    << LOG_TAG("TargetId", ToString(target_id));

  auto weak_ptr = weak_factory_.GetWeakPtr();
  service_.DeleteReference(
      reference_type_id, source_id, target_id,
      [this, weak_ptr, request_id](const scada::Status& status) {
        auto ptr = weak_ptr.get();
        if (!ptr)
          return;

        LOG_INFO(logger_) << "Delete reference"
                          << LOG_TAG("RequestId", request_id)
                          << LOG_TAG("Status", ToString(status));

        protocol::Message message;
        auto& response = *message.add_responses();
        response.set_request_id(request_id);
        Convert(status, *response.mutable_status());
        sender_.Send(message);
      });
}
