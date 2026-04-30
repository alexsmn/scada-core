#pragma once

#include "base/any_executor.h"
#include "base/awaitable.h"
#include "base/executor_conversions.h"
#include "base/struct_writer.h"
#include "scada/callback_awaitable.h"
#include "scada/node_attributes.h"
#include "scada/node_class.h"
#include "scada/status.h"
#include "scada/status_callback.h"
#include "scada/status_exception.h"
#include "scada/status_or.h"

#include <functional>
#include <memory>
#include <utility>
#include <vector>

namespace scada {

struct AddNodesItem {
  NodeId requested_id;
  NodeId parent_id;
  NodeClass node_class = NodeClass::Object;
  NodeId type_definition_id;
  NodeAttributes attributes;
};

struct AddNodesResult {
  StatusCode status_code = StatusCode::Good;
  NodeId added_node_id;
};

struct DeleteNodesItem {
  NodeId node_id;
  bool delete_target_references = false;
};

struct AddReferencesItem {
  NodeId source_node_id;
  NodeId reference_type_id;
  bool forward = true;
  String target_server_uri;
  ExpandedNodeId target_node_id;
  NodeClass target_node_class = NodeClass::Object;
};

struct DeleteReferencesItem {
  NodeId source_node_id;
  NodeId reference_type_id;
  bool forward = true;
  ExpandedNodeId target_node_id;
  bool delete_bidirectional = true;
};

using AddNodesCallback =
    std::function<void(Status status, std::vector<AddNodesResult> results)>;
using DeleteNodesCallback =
    std::function<void(Status status, std::vector<StatusCode> results)>;

using AddReferencesCallback = MultiStatusCallback;
using DeleteReferencesCallback = MultiStatusCallback;

class NodeManagementService {
 public:
  virtual ~NodeManagementService() = default;

  virtual void AddNodes(const std::vector<AddNodesItem>& inputs,
                        const AddNodesCallback& callback) = 0;

  // Delete record from table. If |return_dependencies| is true and deletion
  // fails, it gets list of related records, which must be deleted before.
  virtual void DeleteNodes(const std::vector<DeleteNodesItem>& inputs,
                           const DeleteNodesCallback& callback) = 0;

  virtual void AddReferences(const std::vector<AddReferencesItem>& inputs,
                             const AddReferencesCallback& callback) = 0;

  virtual void DeleteReferences(const std::vector<DeleteReferencesItem>& inputs,
                                const DeleteReferencesCallback& callback) = 0;
};

class CoroutineNodeManagementService {
 public:
  virtual ~CoroutineNodeManagementService() = default;

  virtual Awaitable<StatusOr<std::vector<AddNodesResult>>> AddNodes(
      std::vector<AddNodesItem> inputs) = 0;

  virtual Awaitable<StatusOr<std::vector<StatusCode>>> DeleteNodes(
      std::vector<DeleteNodesItem> inputs) = 0;

  virtual Awaitable<StatusOr<std::vector<StatusCode>>>
  AddReferences(std::vector<AddReferencesItem> inputs) = 0;

  virtual Awaitable<StatusOr<std::vector<StatusCode>>>
  DeleteReferences(std::vector<DeleteReferencesItem> inputs) = 0;
};

class CallbackToCoroutineNodeManagementServiceAdapter final
    : public CoroutineNodeManagementService {
 public:
  CallbackToCoroutineNodeManagementServiceAdapter(
      AnyExecutor executor,
      NodeManagementService& service)
      : executor_{std::move(executor)}, service_{service} {}
  CallbackToCoroutineNodeManagementServiceAdapter(
      std::shared_ptr<Executor> executor,
      NodeManagementService& service)
      : CallbackToCoroutineNodeManagementServiceAdapter(
            MakeAnyExecutor(std::move(executor)), service) {}

  Awaitable<StatusOr<std::vector<AddNodesResult>>> AddNodes(
      std::vector<AddNodesItem> inputs) override {
    co_return co_await AwaitStatusOrCallback<std::vector<AddNodesResult>>(
        executor_,
        [this, inputs = std::move(inputs)](auto callback) mutable {
          service_.AddNodes(inputs, std::move(callback));
        });
  }

  Awaitable<StatusOr<std::vector<StatusCode>>> DeleteNodes(
      std::vector<DeleteNodesItem> inputs) override {
    co_return co_await AwaitStatusOrCallback<std::vector<StatusCode>>(
        executor_,
        [this, inputs = std::move(inputs)](auto callback) mutable {
          service_.DeleteNodes(inputs, std::move(callback));
        });
  }

  Awaitable<StatusOr<std::vector<StatusCode>>> AddReferences(
      std::vector<AddReferencesItem> inputs) override {
    co_return co_await AwaitStatusOrCallback<std::vector<StatusCode>>(
        executor_,
        [this, inputs = std::move(inputs)](auto callback) mutable {
          service_.AddReferences(inputs, std::move(callback));
        });
  }

  Awaitable<StatusOr<std::vector<StatusCode>>> DeleteReferences(
      std::vector<DeleteReferencesItem> inputs) override {
    co_return co_await AwaitStatusOrCallback<std::vector<StatusCode>>(
        executor_,
        [this, inputs = std::move(inputs)](auto callback) mutable {
          service_.DeleteReferences(inputs, std::move(callback));
        });
  }

 private:
  const AnyExecutor executor_;
  NodeManagementService& service_;
};

class CoroutineToCallbackNodeManagementServiceAdapter final
    : public NodeManagementService {
 public:
  CoroutineToCallbackNodeManagementServiceAdapter(
      AnyExecutor executor,
      CoroutineNodeManagementService& service)
      : executor_{std::move(executor)}, service_{service} {}
  CoroutineToCallbackNodeManagementServiceAdapter(
      std::shared_ptr<Executor> executor,
      CoroutineNodeManagementService& service)
      : CoroutineToCallbackNodeManagementServiceAdapter(
            MakeAnyExecutor(std::move(executor)), service) {}

  void AddNodes(const std::vector<AddNodesItem>& inputs,
                const AddNodesCallback& callback) override {
    CoSpawn(executor_,
            [this, inputs, callback]() mutable -> Awaitable<void> {
              try {
                CompleteStatusOrCallback(callback,
                                         co_await service_.AddNodes(inputs));
              } catch (...) {
                callback(GetExceptionStatus(std::current_exception()), {});
              }
            });
  }

  void DeleteNodes(const std::vector<DeleteNodesItem>& inputs,
                   const DeleteNodesCallback& callback) override {
    CoSpawn(executor_,
            [this, inputs, callback]() mutable -> Awaitable<void> {
              try {
                CompleteStatusOrCallback(callback,
                                         co_await service_.DeleteNodes(inputs));
              } catch (...) {
                callback(GetExceptionStatus(std::current_exception()), {});
              }
            });
  }

  void AddReferences(const std::vector<AddReferencesItem>& inputs,
                     const AddReferencesCallback& callback) override {
    CoSpawn(executor_,
            [this, inputs, callback]() mutable -> Awaitable<void> {
              try {
                CompleteStatusOrCallback(
                    callback, co_await service_.AddReferences(inputs));
              } catch (...) {
                auto status = GetExceptionStatus(std::current_exception());
                callback(std::move(status), {});
              }
            });
  }

  void DeleteReferences(const std::vector<DeleteReferencesItem>& inputs,
                        const DeleteReferencesCallback& callback) override {
    CoSpawn(executor_,
            [this, inputs, callback]() mutable -> Awaitable<void> {
              try {
                CompleteStatusOrCallback(
                    callback, co_await service_.DeleteReferences(inputs));
              } catch (...) {
                auto status = GetExceptionStatus(std::current_exception());
                callback(std::move(status), {});
              }
            });
  }

 private:
  const AnyExecutor executor_;
  CoroutineNodeManagementService& service_;
};

// using Callback = std::function<void(AddNodesResult&& result)>
template <class Callback>
inline void AddNode(NodeManagementService& service,
                    const AddNodesItem& input,
                    Callback&& callback) {
  service.AddNodes(
      {input}, [callback = std::forward<Callback>(callback)](
                   Status status, std::vector<AddNodesResult> results) mutable {
        assert(results.size() == 1);
        auto result = status ? std::move(results[0])
                             : AddNodesResult{.status_code = status.code()};
        callback(std::move(result));
      });
}

// using Callback = std::function<void(Status&& result)>
template <class Callback>
inline void DeleteNode(NodeManagementService& service,
                       const DeleteNodesItem& input,
                       Callback&& callback) {
  service.DeleteNodes(
      {input},
      [callback](Status&& status, std::vector<StatusCode>&& results) mutable {
        auto result = status ? scada::Status{results[0]} : std::move(status);
        callback(std::move(result));
      });
}

inline bool operator==(const AddNodesItem& a, const AddNodesItem& b) {
  return a.requested_id == b.requested_id && a.parent_id == b.parent_id &&
         a.node_class == b.node_class &&
         a.type_definition_id == b.type_definition_id &&
         a.attributes == b.attributes;
}

inline std::ostream& operator<<(std::ostream& stream,
                                const AddNodesItem& item) {
  StructWriter{stream}
      .AddField("requested_id", item.requested_id)
      .AddField("parent_id", item.parent_id)
      .AddField("node_class", item.node_class)
      .AddField("type_definition_id", item.type_definition_id)
      .AddField("attributes", item.attributes);
  return stream;
}

}  // namespace scada
