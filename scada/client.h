#pragma once

#include "base/check.h"
#include "base/lifetime.h"
#include "scada/node.h"
#include "scada/session_service.h"
#include "scada/status_or.h"

namespace scada {

class ServiceContext;
struct AddNodesItem;

class client {
 public:
  client();
  explicit client(const services& services);

  const ServiceContext& context() const SCADA_LIFETIME_BOUND {
    return context_;
  }
  client with_context(const ServiceContext& context) const;

  Awaitable<Status> connect(SessionConnectParams params) const;
  Awaitable<Status> disconnect() const;

  scada::node node(const NodeId& node_id) const {
    base::Check(!node_id.is_null());
    return scada::node{services_, node_id, context_};
  }

  scada::node server_node() const { return node(id::Server); }

  // Lazy coroutine: `inputs` is taken by value so the coroutine frame owns
  // the vector; a const& bound to a caller temporary would dangle.
  Awaitable<StatusOr<std::vector<StatusOr<std::vector<ReferenceDescription>>>>>
  browse(std::vector<BrowseDescription> inputs) const;

  Awaitable<StatusOr<scada::node>> add_node(AddNodesItem item) const;

  Awaitable<Status> acknowledge_events(std::vector<EventId> event_ids,
                                       DateTime acknowledge_time) const;

  Awaitable<Status> acknowledge_event(EventId event_id,
                                      DateTime acknowledge_time) const {
    return acknowledge_events({event_id}, acknowledge_time);
  }

 private:
  client(const services& services, const ServiceContext& context)
      : services_{services}, context_{context} {}

  services services_;
  scada::ServiceContext context_;
};

}  // namespace scada
