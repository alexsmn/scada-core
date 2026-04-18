#include "remote/node_management_stub.h"

#include "base/awaitable.h"
#include "base/executor.h"
#include "remote/message_sender.h"
#include "remote/protocol.h"
#include "remote/protocol_utils.h"
#include "scada/node_management_service.h"
#include "scada/service_awaitable.h"

#include "base/debug_util.h"

namespace {

inline bool ContainsNodeId(const std::vector<scada::DeleteNodesItem>& inputs,
                           const scada::NodeId& node_id) {
  return std::any_of(inputs.begin(), inputs.end(),
                     [&](auto& input) { return input.node_id == node_id; });
}

}  // namespace

NodeManagementStub::NodeManagementStub(
    std::shared_ptr<Executor> executor,
    std::weak_ptr<MessageSender> sender,
    scada::NodeManagementService& service,
    const scada::ServiceContext& service_context)
    : executor_{std::move(executor)},
      sender_{std::move(sender)},
      service_{service},
      service_context_{service_context} {}

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
  // TODO: Fail only for |service_context_->user_id|.
  if (ContainsNodeId(inputs, service_context_.user_id())) {
    protocol::Message message;
    auto& response = *message.add_responses();
    response.set_request_id(request_id);
    auto& delete_node_result = *response.mutable_delete_node_result();
    delete_node_result;
    Convert(scada::Status{scada::StatusCode::Bad_CantDeleteOwnUser},
            *response.mutable_status());
    if (auto locked_sender = sender_.lock())
      locked_sender->Send(message);
    return;
  }
  auto self = shared_from_this();
  CoSpawn(
      executor_,
      [self, request_id, inputs]() mutable -> Awaitable<void> {
        co_await self->OnDeleteNodesAsync(request_id, std::move(inputs));
      });
}

void NodeManagementStub::OnAddNodes(
    unsigned request_id,
  const std::vector<scada::AddNodesItem>& inputs) {
  auto self = shared_from_this();
  CoSpawn(
      executor_,
      [self, request_id, inputs]() mutable -> Awaitable<void> {
        co_await self->OnAddNodesAsync(request_id, std::move(inputs));
      });
}

void NodeManagementStub::OnAddReferences(
    unsigned request_id,
    const std::vector<scada::AddReferencesItem>& inputs) {
  LOG_INFO(*logger_) << "Add references" << LOG_TAG("RequestId", request_id)
                     << LOG_TAG("Count", inputs.size());
  auto self = shared_from_this();
  CoSpawn(
      executor_,
      [self, request_id, inputs]() mutable -> Awaitable<void> {
        co_await self->OnAddReferencesAsync(request_id, std::move(inputs));
      });
}

void NodeManagementStub::OnDeleteReferences(
    unsigned request_id,
    const std::vector<scada::DeleteReferencesItem>& inputs) {
  LOG_INFO(*logger_) << "Delete reference" << LOG_TAG("RequestId", request_id)
                     << LOG_TAG("Count", inputs.size());
  auto self = shared_from_this();
  CoSpawn(
      executor_,
      [self, request_id, inputs]() mutable -> Awaitable<void> {
        co_await self->OnDeleteReferencesAsync(request_id, std::move(inputs));
      });
}

Awaitable<void> NodeManagementStub::OnDeleteNodesAsync(
    unsigned request_id,
    std::vector<scada::DeleteNodesItem> inputs) {
  auto [status, results] =
      co_await scada::DeleteNodesAsync(executor_, service_, std::move(inputs));

  protocol::Message message;
  auto& response = *message.add_responses();
  response.set_request_id(request_id);
  Convert(status, *response.mutable_status());
  if (status) {
    Convert(results, *response.mutable_delete_node_result());
  }

  if (auto locked_sender = sender_.lock())
    locked_sender->Send(message);
}

Awaitable<void> NodeManagementStub::OnAddNodesAsync(
    unsigned request_id,
    std::vector<scada::AddNodesItem> inputs) {
  auto [status, results] =
      co_await scada::AddNodesAsync(executor_, service_, std::move(inputs));

  protocol::Message message;
  auto& response = *message.add_responses();
  response.set_request_id(request_id);
  Convert(status, *response.mutable_status());
  if (status) {
    Convert(std::move(results), *response.mutable_add_node_result());
  }

  if (auto locked_sender = sender_.lock())
    locked_sender->Send(message);
}

Awaitable<void> NodeManagementStub::OnAddReferencesAsync(
    unsigned request_id,
    std::vector<scada::AddReferencesItem> inputs) {
  const auto count = inputs.size();
  auto [status, results] = co_await scada::AddReferencesAsync(
      executor_, service_, std::move(inputs));

  LOG_INFO(*logger_) << "Add references completed"
                     << LOG_TAG("RequestId", request_id)
                     << LOG_TAG("Count", count)
                     << LOG_TAG("Status", ToString(status));

  protocol::Message message;
  auto& response = *message.add_responses();
  response.set_request_id(request_id);
  Convert(status, *response.mutable_status());
  Convert(results, *response.mutable_add_reference_result());

  if (auto locked_sender = sender_.lock())
    locked_sender->Send(message);
}

Awaitable<void> NodeManagementStub::OnDeleteReferencesAsync(
    unsigned request_id,
    std::vector<scada::DeleteReferencesItem> inputs) {
  const auto count = inputs.size();
  auto [status, results] = co_await scada::DeleteReferencesAsync(
      executor_, service_, std::move(inputs));

  LOG_INFO(*logger_) << "Delete references completed"
                     << LOG_TAG("RequestId", request_id)
                     << LOG_TAG("Count", count)
                     << LOG_TAG("Status", ToString(status));

  protocol::Message message;
  auto& response = *message.add_responses();
  response.set_request_id(request_id);
  Convert(status, *response.mutable_status());
  Convert(results, *response.mutable_delete_reference_result());

  if (auto locked_sender = sender_.lock())
    locked_sender->Send(message);
}
