#include "remote/node_management_stub.h"

#include "base/strings/sys_string_conversions.h"
#include "base/strings/utf_string_conversions.h"
#include "core/node_management_service.h"
#include "remote/message_sender.h"
#include "remote/protocol.h"
#include "remote/protocol_utils.h"

NodeManagementStub::NodeManagementStub(MessageSender& sender,
                                       scada::NodeManagementService& service,
                                       const scada::NodeId& user_id,
                                       std::shared_ptr<Logger> logger)
    : sender_(sender),
      service_(service),
      user_id_(user_id),
      logger_{std::move(logger)} {}

void NodeManagementStub::OnRequestReceived(const protocol::Request& request) {
  if (request.has_create_node()) {
    auto& create_node = request.create_node();
    OnCreateNode(
        request.request_id(),
        create_node.has_requested_node_id()
            ? FromProto(create_node.requested_node_id())
            : scada::NodeId(),
        FromProto(create_node.parent_id()), FromProto(create_node.node_class()),
        FromProto(create_node.type_definition_id()),
        create_node.has_attributes() ? FromProto(create_node.attributes())
                                     : scada::NodeAttributes());
  }

  if (request.has_delete_node()) {
    auto& delete_node = request.delete_node();
    OnDeleteNode(request.request_id(), FromProto(delete_node.node_id()),
                 delete_node.return_dependencies());
  }

  if (request.has_change_password()) {
    auto& change_password = request.change_password();
    OnChangeUserPassword(
        request.request_id(), FromProto(change_password.user_node_id()),
        base::UTF8ToUTF16(change_password.current_password_utf8()),
        base::UTF8ToUTF16(change_password.new_password_utf8()));
  }

  if (request.add_reference_size() != 0) {
    OnAddReferences(
        request.request_id(),
        VectorFromProto<scada::AddReferencesItem>(request.add_reference()));
  }

  if (request.delete_reference_size() != 0) {
    OnDeleteReferences(request.request_id(),
                       VectorFromProto<scada::DeleteReferencesItem>(
                           request.delete_reference()));
  }
}

void NodeManagementStub::OnDeleteNode(unsigned request_id,
                                      const scada::NodeId& id,
                                      bool return_dependencies) {
  if (id == user_id_) {
    protocol::Message message;
    auto& response = *message.add_responses();
    response.set_request_id(request_id);
    auto& delete_node_result = *response.mutable_delete_node_result();
    ToProto(scada::StatusCode::Bad_CantDeleteOwnUser,
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
        ToProto(status, *response.mutable_status());
        if (!dependencies.empty())
          ContainerToProto(dependencies,
                           *delete_node_result.mutable_dependencies());
        ptr->sender_.Send(message);
      });
}

void NodeManagementStub::OnCreateNode(unsigned request_id,
                                      const scada::NodeId& requested_id,
                                      const scada::NodeId& parent_id,
                                      scada::NodeClass node_class,
                                      const scada::NodeId& type_id,
                                      scada::NodeAttributes attributes) {
  auto weak_ptr = weak_factory_.GetWeakPtr();
  service_.CreateNode(
      requested_id, parent_id, node_class, type_id, std::move(attributes),
      [weak_ptr, request_id](const scada::Status& status,
                             const scada::NodeId& node_id) {
        auto ptr = weak_ptr.get();
        if (!ptr)
          return;

        protocol::Message message;
        auto& response = *message.add_responses();
        response.set_request_id(request_id);
        auto& create_node_result = *response.mutable_create_node_result();
        ToProto(status, *response.mutable_status());
        ToProto(node_id, *create_node_result.mutable_node_id());
        ptr->sender_.Send(message);
      });
}

void NodeManagementStub::OnChangeUserPassword(
    unsigned request_id,
    const scada::NodeId& user_node_id,
    const scada::LocalizedText& current_password,
    const scada::LocalizedText& new_password) {
  auto weak_ptr = weak_factory_.GetWeakPtr();
  service_.ChangeUserPassword(
      user_node_id, current_password, new_password,
      [weak_ptr, request_id](const scada::Status& status) {
        auto ptr = weak_ptr.get();
        if (!ptr)
          return;

        protocol::Message message;
        auto& response = *message.add_responses();
        response.set_request_id(request_id);
        ToProto(status, *response.mutable_status());
        ptr->sender_.Send(message);
      });
}

void NodeManagementStub::OnAddReferences(
    unsigned request_id,
    const std::vector<scada::AddReferencesItem>& inputs) {
  auto weak_ptr = weak_factory_.GetWeakPtr();
  service_.AddReferences(
      inputs,
      [weak_ptr, request_id](const scada::Status& status,
                             const std::vector<scada::Status>& results) {
        auto ptr = weak_ptr.get();
        if (!ptr)
          return;

        protocol::Message message;
        auto& response = *message.add_responses();
        response.set_request_id(request_id);
        ToProto(status, *response.mutable_status());
        ContainerToProto(results, *response.mutable_add_reference_result());
        ptr->sender_.Send(message);
      });
}

void NodeManagementStub::OnDeleteReferences(
    unsigned request_id,
    const std::vector<scada::DeleteReferencesItem>& inputs) {
  auto weak_ptr = weak_factory_.GetWeakPtr();
  service_.DeleteReferences(
      inputs,
      [weak_ptr, request_id](const scada::Status& status,
                             const std::vector<scada::Status>& results) {
        auto ptr = weak_ptr.get();
        if (!ptr)
          return;

        protocol::Message message;
        auto& response = *message.add_responses();
        response.set_request_id(request_id);
        ToProto(status, *response.mutable_status());
        ContainerToProto(results, *response.mutable_delete_reference_result());
        ptr->sender_.Send(message);
      });
}
