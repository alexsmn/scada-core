#include "scada/node.h"

#include "scada/attribute_service_promises.h"
#include "scada/history_service_promises.h"
#include "scada/method_service_promises.h"
#include "scada/view_service_promises.h"

namespace scada {

// node

promise<DataValue> node::read(AttributeId attribute_id) const {
  if (!services_.attribute_service) {
    return MakeRejectedStatusPromise<DataValue>(StatusCode::Bad_Disconnected);
  }

  return Read(*services_.attribute_service, context_,
              {.node_id = node_id_, .attribute_id = attribute_id})
      .then([](const scada::DataValue& result) {
        return scada::IsBad(result.status_code)
                   ? MakeRejectedStatusPromise<DataValue>(result.status_code)
                   : make_resolved_promise(result);
      });
}

promise<> node::write(AttributeId attribute_id,
                      const Variant& value,
                      scada::WriteFlags flags) const {
  if (!services_.attribute_service) {
    return MakeRejectedStatusPromise(StatusCode::Bad);
  }

  return ToStatusPromise(Write(*services_.attribute_service, context_,
                               {.node_id = node_id_,
                                .attribute_id = attribute_id,
                                .value = value,
                                .flags = flags}));
}

promise<std::vector<ReferenceDescription>> node::browse(
    const browse_details& details) const {
  if (!services_.view_service) {
    return MakeRejectedStatusPromise<std::vector<ReferenceDescription>>(
        StatusCode::Bad_Disconnected);
  }

  promise<BrowseResult> promise;

  Browse(*services_.view_service,
         {.node_id = node_id_,
          .direction = details.direction,
          .reference_type_id = details.reference_type_id},
         MakeStatusCodePromiseCallback(promise));

  return promise.then([](const BrowseResult& result) {
    assert(IsGood(result.status_code));
    return result.references;
  });
}

promise<scada::node> node::browse_node(const browse_details& details) const {
  return browse(details).then(
      [*this](const std::vector<ReferenceDescription>& results) {
        return results.size() == 1
                   ? make_resolved_promise(
                         scada::node{services_, results[0].node_id, context_})
                   : MakeRejectedStatusPromise<scada::node>(StatusCode::Bad);
      });
}

promise<std::vector<BrowsePathTarget>> node::translate_browse_path(
    const RelativePath& relative_path) const {
  if (!services_.view_service) {
    return MakeRejectedStatusPromise<std::vector<BrowsePathTarget>>(
        StatusCode::Bad_Disconnected);
  }

  promise<BrowsePathResult> promise;

  TranslateBrowsePath(*services_.view_service,
                      {.node_id = node_id_, .relative_path = relative_path},
                      MakeStatusCodePromiseCallback(promise));

  return promise.then([](const BrowsePathResult& result) {
    assert(IsGood(result.status_code));
    return result.targets;
  });
}

promise<NodeId> node::child_id(scada::QualifiedName browse_name) const {
  return translate_browse_path({{.reference_type_id = id::HasChild,
                                 .target_name = std::move(browse_name)}})
      .then([](std::vector<BrowsePathTarget> targets) {
        return targets.size() == 1
                   ? make_resolved_promise(targets[0].target_id.node_id())
                   : MakeRejectedStatusPromise<NodeId>(
                         StatusCode::Bad_BrowseNameInvalid);
      });
}

promise<node> node::child_node(scada::QualifiedName browse_name) const {
  return child_id(std::move(browse_name))
      .then([services = services_,
             context = context_](const NodeId& node_id) mutable {
        return scada::node{std::move(services), node_id, std::move(context)};
      });
}

promise<> node::call_packed(const NodeId& method_id,
                            const std::vector<Variant>& arguments) const {
  if (!services_.method_service) {
    return MakeRejectedStatusPromise(StatusCode::Bad_Disconnected);
  }

  // Take `user_id` from `client`.
  return ToStatusPromise(Call(*services_.method_service, node_id_, method_id,
                              arguments, context_->user_id));
}

promise<std::vector<scada::DataValue>> node::read_value_history(
    const HistoryReadRawDetails& details) const {
  assert(details.node_id.is_null());
  assert(details.continuation_point.empty());

  if (!services_.history_service) {
    return MakeRejectedStatusPromise<std::vector<scada::DataValue>>(
        StatusCode::Bad_Disconnected);
  }

  auto sanitized_details = details;
  sanitized_details.node_id = node_id_;

  return HistoryReadRaw(*services_.history_service, sanitized_details);
}

promise<HistoryReadRawResult> node::read_value_history_chunk(
    const HistoryReadRawDetails& details) const {
  assert(details.node_id.is_null());

  if (!services_.history_service) {
    return MakeRejectedStatusPromise<HistoryReadRawResult>(
        StatusCode::Bad_Disconnected);
  }

  auto sanitized_details = details;
  sanitized_details.node_id = node_id_;

  return HistoryReadRawChunk(*services_.history_service, sanitized_details);
}

promise<std::vector<Event>> node::read_event_history(
    const event_history_details& details) const {
  if (!services_.history_service) {
    return MakeRejectedStatusPromise<std::vector<Event>>(
        StatusCode::Bad_Disconnected);
  }

  return HistoryReadEvents(*services_.history_service, node_id_, details.from,
                           details.to, details.filter)
      .then([](const scada::HistoryReadEventsResult& result) {
        return result.events;
      });
}

}  // namespace scada