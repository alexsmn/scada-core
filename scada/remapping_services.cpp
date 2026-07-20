#include "scada/remapping_services.h"

#include <memory>
#include <span>
#include <utility>
#include <variant>
#include <vector>

namespace scada::aggregation {

Awaitable<scada::StatusOr<std::vector<scada::BrowseResult>>>
RemappingViewService::Browse(scada::ServiceContext context,
                             std::vector<scada::BrowseDescription> inputs) {
  for (auto& description : inputs) {
    description.node_id = remapper_.ToDownstream(description.node_id);
    description.reference_type_id =
        remapper_.ToDownstream(description.reference_type_id);
  }
  auto result = co_await inner_.Browse(std::move(context), std::move(inputs));
  if (result.ok()) {
    for (auto& browse_result : *result) {
      for (auto& reference : browse_result.references) {
        reference = remapper_.ToProxy(reference);
      }
    }
  }
  co_return result;
}

Awaitable<scada::StatusOr<std::vector<scada::BrowsePathResult>>>
RemappingViewService::TranslateBrowsePaths(
    std::vector<scada::BrowsePath> inputs) {
  for (auto& path : inputs) {
    path.node_id = remapper_.ToDownstream(path.node_id);
    for (auto& element : path.relative_path) {
      element.reference_type_id =
          remapper_.ToDownstream(element.reference_type_id);
      element.target_name = remapper_.ToDownstream(element.target_name);
    }
  }
  auto result = co_await inner_.TranslateBrowsePaths(std::move(inputs));
  if (result.ok()) {
    for (auto& path_result : *result) {
      for (auto& target : path_result.targets) {
        target.target_id = remapper_.ToProxy(target.target_id);
      }
    }
  }
  co_return result;
}

Awaitable<scada::StatusOr<std::vector<scada::DataValue>>>
RemappingAttributeService::Read(
    scada::ServiceContext context,
    std::vector<scada::ReadValueId> inputs) {
  for (auto& read : inputs) {
    read.node_id = remapper_.ToDownstream(read.node_id);
  }
  auto result = co_await inner_.Read(std::move(context), std::move(inputs));
  if (result.ok()) {
    // Identifier-typed values (e.g. a DataType or BrowseName attribute) come
    // back in downstream namespaces; translate them to proxy-local ones.
    for (auto& data_value : *result) {
      data_value.value = remapper_.ToProxy(data_value.value);
    }
  }
  co_return result;
}

Awaitable<scada::StatusOr<std::vector<scada::StatusCode>>>
RemappingAttributeService::Write(
    scada::ServiceContext context,
    std::vector<scada::WriteValue> inputs) {
  for (auto& write : inputs) {
    write.node_id = remapper_.ToDownstream(write.node_id);
    write.value = remapper_.ToDownstream(write.value);
  }
  co_return co_await inner_.Write(std::move(context), std::move(inputs));
}

Awaitable<scada::HistoryReadRawResult> RemappingHistoryService::HistoryReadRaw(
    scada::HistoryReadRawDetails details) {
  details.node_id = remapper_.ToDownstream(details.node_id);
  auto result = co_await inner_.HistoryReadRaw(std::move(details));
  // Identifier-typed historical values (e.g. a node reference stored as a value)
  // come back in downstream namespaces; translate them to proxy-local ones, as
  // the attribute Read path does.
  for (auto& data_value : result.values) {
    data_value.value = remapper_.ToProxy(data_value.value);
  }
  co_return result;
}

Awaitable<scada::HistoryReadEventsResult>
RemappingHistoryService::HistoryReadEvents(scada::NodeId node_id,
                                           base::Time from,
                                           base::Time to,
                                           scada::EventFilter filter) {
  // Only the source NodeId lives in the downstream's namespaces here. Event
  // payloads (std::any) are not yet remapped — the same untranslated surface as
  // the monitored-item event notifications.
  co_return co_await inner_.HistoryReadEvents(remapper_.ToDownstream(node_id),
                                              from, to, std::move(filter));
}

Awaitable<scada::StatusOr<std::vector<scada::StatusCode>>>
RemappingHistoryUpdateService::HistoryUpdateData(
    scada::ServiceContext context,
    scada::UpdateDataDetails details) {
  details.node_id = remapper_.ToDownstream(details.node_id);
  // Identifier-typed values being written (a node reference stored as a value)
  // arrive in proxy namespaces; translate them like the attribute Write path.
  for (auto& data_value : details.values) {
    data_value.value = remapper_.ToDownstream(data_value.value);
  }
  co_return co_await inner_.HistoryUpdateData(std::move(context),
                                              std::move(details));
}

Awaitable<scada::StatusOr<std::vector<scada::StatusCode>>>
RemappingHistoryUpdateService::HistoryUpdateEvent(
    scada::ServiceContext context,
    scada::UpdateEventDetails details) {
  // Only the source NodeId lives in the remapped namespaces here. Event
  // payloads (std::any) are not translated — the same untranslated surface as
  // the monitored-item and history-read event notifications.
  details.node_id = remapper_.ToDownstream(details.node_id);
  co_return co_await inner_.HistoryUpdateEvent(std::move(context),
                                               std::move(details));
}

Awaitable<scada::Status> RemappingMethodService::Call(
    scada::NodeId node_id,
    scada::NodeId method_id,
    std::vector<scada::Variant> arguments,
    scada::ServiceContext context) {
  // The object and method live in the downstream's namespaces, as do any
  // identifier-typed input argument values. The caller's context (user id +
  // rights) passes through unchanged.
  for (auto& argument : arguments) {
    argument = remapper_.ToDownstream(argument);
  }
  co_return co_await inner_.Call(remapper_.ToDownstream(node_id),
                                 remapper_.ToDownstream(method_id),
                                 std::move(arguments), std::move(context));
}

Awaitable<scada::StatusOr<std::vector<scada::AddNodesResult>>>
RemappingNodeManagementService::AddNodes(
    scada::ServiceContext context,
    std::vector<scada::AddNodesItem> inputs) {
  for (auto& item : inputs) {
    item.requested_id = remapper_.ToDownstream(item.requested_id);
    item.parent_id = remapper_.ToDownstream(item.parent_id);
    item.type_definition_id = remapper_.ToDownstream(item.type_definition_id);
    // Identifier-typed attributes (browse name, data type, an identifier
    // carried as the initial value) live in the proxy's namespaces too.
    item.attributes.browse_name =
        remapper_.ToDownstream(item.attributes.browse_name);
    item.attributes.data_type = remapper_.ToDownstream(item.attributes.data_type);
    if (item.attributes.value) {
      item.attributes.value = remapper_.ToDownstream(*item.attributes.value);
    }
  }
  auto result = co_await inner_.AddNodes(std::move(context), std::move(inputs));
  if (result.ok()) {
    for (auto& added : *result) {
      added.added_node_id = remapper_.ToProxy(added.added_node_id);
    }
  }
  co_return result;
}

Awaitable<scada::StatusOr<std::vector<scada::StatusCode>>>
RemappingNodeManagementService::DeleteNodes(
    scada::ServiceContext context,
    std::vector<scada::DeleteNodesItem> inputs) {
  for (auto& item : inputs) {
    item.node_id = remapper_.ToDownstream(item.node_id);
  }
  co_return co_await inner_.DeleteNodes(std::move(context), std::move(inputs));
}

Awaitable<scada::StatusOr<std::vector<scada::StatusCode>>>
RemappingNodeManagementService::AddReferences(
    scada::ServiceContext context,
    std::vector<scada::AddReferencesItem> inputs) {
  for (auto& item : inputs) {
    item.source_node_id = remapper_.ToDownstream(item.source_node_id);
    item.reference_type_id = remapper_.ToDownstream(item.reference_type_id);
    item.target_node_id = remapper_.ToDownstream(item.target_node_id);
  }
  co_return co_await inner_.AddReferences(std::move(context),
                                          std::move(inputs));
}

Awaitable<scada::StatusOr<std::vector<scada::StatusCode>>>
RemappingNodeManagementService::DeleteReferences(
    scada::ServiceContext context,
    std::vector<scada::DeleteReferencesItem> inputs) {
  for (auto& item : inputs) {
    item.source_node_id = remapper_.ToDownstream(item.source_node_id);
    item.reference_type_id = remapper_.ToDownstream(item.reference_type_id);
    item.target_node_id = remapper_.ToDownstream(item.target_node_id);
  }
  co_return co_await inner_.DeleteReferences(std::move(context),
                                             std::move(inputs));
}

namespace {

// A subscription that remaps each added item's target NodeId proxy -> downstream
// and otherwise delegates to the downstream subscription. Removals and reads key
// on item id / client handle, so they need no translation.
class RemappingMonitoredItemSubscription final
    : public scada::MonitoredItemSubscription {
 public:
  RemappingMonitoredItemSubscription(
      std::unique_ptr<scada::MonitoredItemSubscription> inner,
      const NamespaceRemapper& remapper)
      : inner_{std::move(inner)}, remapper_{remapper} {}

  Awaitable<std::vector<scada::MonitoredItemCreateResult>> AddItems(
      std::vector<scada::MonitoredItemCreateRequest> requests) override {
    for (auto& request : requests) {
      request.item_to_monitor.node_id =
          remapper_.ToDownstream(request.item_to_monitor.node_id);
    }
    co_return co_await inner_->AddItems(std::move(requests));
  }

  Awaitable<std::vector<scada::Status>> RemoveItems(
      std::span<const scada::MonitoredItemId> item_ids) override {
    co_return co_await inner_->RemoveItems(item_ids);
  }

  Awaitable<scada::StatusOr<std::vector<scada::MonitoredItemNotification>>>
  ReadNext(std::size_t max_count) override {
    auto result = co_await inner_->ReadNext(max_count);
    if (result.ok()) {
      // Data-change values may themselves be identifiers; translate them to
      // proxy-local namespaces. Event payloads (std::any) are not yet remapped.
      for (auto& notification : *result) {
        if (auto* change =
                std::get_if<scada::DataChangeNotification>(&notification)) {
          change->value.value = remapper_.ToProxy(change->value.value);
        }
      }
    }
    co_return result;
  }

  void Close(scada::Status status) override { inner_->Close(status); }

 private:
  std::unique_ptr<scada::MonitoredItemSubscription> inner_;
  const NamespaceRemapper& remapper_;
};

}  // namespace

scada::StatusOr<std::unique_ptr<scada::MonitoredItemSubscription>>
RemappingMonitoredItemService::CreateSubscription(
    scada::ServiceContext context,
    scada::MonitoredItemSubscriptionOptions options) {
  auto inner = inner_.CreateSubscription(std::move(context), options);
  if (!inner.ok()) {
    return inner.status();
  }
  std::unique_ptr<scada::MonitoredItemSubscription> wrapped =
      std::make_unique<RemappingMonitoredItemSubscription>(std::move(*inner),
                                                           remapper_);
  return wrapped;
}

Awaitable<scada::StatusOr<std::vector<scada::BrowseResult>>>
MountViewService::Browse(scada::ServiceContext context,
                         std::vector<scada::BrowseDescription> inputs) {
  auto result = co_await inner_.Browse(std::move(context), std::move(inputs));
  if (result.ok()) {
    for (auto& browse_result : *result) {
      // Keep only the downstream's own-namespace children; the proxy serves the
      // ns 0 standard nodes (Server, Types, ...) itself.
      std::erase_if(browse_result.references, [](const auto& reference) {
        return reference.node_id.namespace_index() == 0;
      });
    }
  }
  co_return result;
}

Awaitable<scada::StatusOr<std::vector<scada::BrowsePathResult>>>
MountViewService::TranslateBrowsePaths(std::vector<scada::BrowsePath> inputs) {
  // Browse paths resolve against the proxy's own Objects folder; the mount only
  // contributes Browse references.
  co_return std::vector<scada::BrowsePathResult>(inputs.size());
}

}  // namespace scada::aggregation
