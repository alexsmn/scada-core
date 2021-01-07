#include "remote/node_management_stub.h"

#include "base/strings/sys_string_conversions.h"
#include "base/strings/utf_string_conversions.h"
#include "core/node_management_service.h"
#include "remote/message_sender.h"
#include "remote/protocol.h"
#include "remote/protocol_utils.h"

namespace {

inline bool ContainsNodeId(const std::vector<scada::DeleteNodesItem>& inputs,
                           const scada::NodeId& node_id) {
  return std::any_of(inputs.begin(), inputs.end(),
                     [&](auto& input) { return input.node_id == node_id; });
}

}  // namespace

NodeManagementStub::NodeManagementStub(MessageSender& sender,
                                       scada::NodeManagementService& service,
                                       const scada::NodeId& user_id,
                                       std::shared_ptr<Logger> logger)
    : sender_(sender),
      service_(service),
      user_id_(user_id),
      logger_{std::move(logger)} {}

void NodeManagementStub::OnRequestReceived(const protocol::Request& request) {
  if (request.add_node_size() != 0) {
    OnAddNodes(request.request_id(),
               ConvertTo<std::vector<scada::AddNodesItem>>(request.add_node()));
  }

  if (request.delete_node_size() != 0) {
    OnDeleteNodes(
        request.request_id(),
        ConvertTo<std::vector<scada::DeleteNodesItem>>(request.delete_node()));
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

  if (request.add_reference_size() != 0) {
    OnAddReferences(request.request_id(),
                    ConvertTo<std::vector<scada::AddReferencesItem>>(
                        request.add_reference()));
  }

  if (request.delete_reference_size() != 0) {
    OnDeleteReferences(request.request_id(),
                       ConvertTo<std::vector<scada::DeleteReferencesItem>>(
                           request.delete_reference()));
  }
}

void NodeManagementStub::OnDeleteNodes(
    unsigned request_id,
    const std::vector<scada::DeleteNodesItem>& inputs) {
  // TODO: Fail only for |user_id_|.
  if (ContainsNodeId(inputs, user_id_)) {
    protocol::Message message;
    auto& response = *message.add_responses();
    response.set_request_id(request_id);
    auto& delete_node_result = *response.mutable_delete_node_result();
    delete_node_result;
    Convert(scada::Status{scada::StatusCode::Bad_CantDeleteOwnUser},
            *response.mutable_status());
    sender_.Send(message);
    return;
  }

  auto weak_ptr = weak_factory_.GetWeakPtr();
  service_.DeleteNodes(
      inputs, [weak_ptr, request_id](scada::Status&& status,
                                     std::vector<scada::StatusCode>&& results) {
        auto ptr = weak_ptr.get();
        if (!ptr)
          return;

        protocol::Message message;
        auto& response = *message.add_responses();
        response.set_request_id(request_id);
        Convert(status, *response.mutable_status());
        if (status)
          Convert(results, *response.mutable_delete_node_result());
        ptr->sender_.Send(message);
      });
}

void NodeManagementStub::OnAddNodes(
    unsigned request_id,
    const std::vector<scada::AddNodesItem>& inputs) {
  auto weak_ptr = weak_factory_.GetWeakPtr();
  service_.AddNodes(inputs, [weak_ptr, request_id](
                                scada::Status&& status,
                                std::vector<scada::AddNodesResult>&& results) {
    auto ptr = weak_ptr.get();
    if (!ptr)
      return;

    protocol::Message message;
    auto& response = *message.add_responses();
    response.set_request_id(request_id);
    Convert(status, *response.mutable_status());
    if (status)
      Convert(std::move(results), *response.mutable_add_node_result());
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
        Convert(status, *response.mutable_status());
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
                             const std::vector<scada::StatusCode>& results) {
        auto ptr = weak_ptr.get();
        if (!ptr)
          return;

        protocol::Message message;
        auto& response = *message.add_responses();
        response.set_request_id(request_id);
        Convert(status, *response.mutable_status());
        Convert(results, *response.mutable_add_reference_result());
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
                             const std::vector<scada::StatusCode>& results) {
        auto ptr = weak_ptr.get();
        if (!ptr)
          return;

        protocol::Message message;
        auto& response = *message.add_responses();
        response.set_request_id(request_id);
        Convert(status, *response.mutable_status());
        Convert(results, *response.mutable_delete_reference_result());
        ptr->sender_.Send(message);
      });
}
