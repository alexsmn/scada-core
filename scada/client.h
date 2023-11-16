#pragma once

#include "scada/node.h"
#include "scada/session_service.h"

namespace scada {

struct AddNodesItem;

class client {
 public:
  client() {}
  explicit client(services services) : services_{std::move(services)} {}

  const ServiceContext& context() const { return *context_; }

  client with_context(ServiceContext context) const {
    return client{services_,
                  std::make_shared<ServiceContext>(std::move(context))};
  }

  promise<> connect(const SessionConnectParams& params) const;

  promise<> disconnect() const;

  scada::node node(const NodeId& node_id) const {
    assert(!node_id.is_null());
    return scada::node{services_, node_id, context_};
  }

  scada::node server_node() const { return node(id::Server); }

  promise<scada::node> add_node(const AddNodesItem& item);

  promise<> acknowledge_events(std::vector<EventId> event_ids,
                               DateTime acknowledge_time) const;

  promise<> acknowledge_event(EventId event_id,
                              DateTime acknowledge_time) const {
    return acknowledge_events({event_id}, acknowledge_time);
  }

 private:
  client(services services, std::shared_ptr<ServiceContext> context)
      : services_{std::move(services)}, context_{std::move(context)} {
    assert(context_);
  }

  services services_;
  ServiceContextPtr context_ = ServiceContext::default_instance();
};

}  // namespace scada
