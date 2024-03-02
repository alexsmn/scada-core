#pragma once

#include "scada/node.h"
#include "scada/session_service.h"

namespace scada {

class ServiceContext;
struct AddNodesItem;

class client {
 public:
  client();
  explicit client(const services& services);

  const ServiceContext& context() const { return context_; }
  client with_context(const ServiceContext& context) const;

  status_promise<void> connect(const SessionConnectParams& params) const;
  status_promise<void> disconnect() const;

  scada::node node(const NodeId& node_id) const {
    assert(!node_id.is_null());
    return scada::node{services_, node_id, context_};
  }

  scada::node server_node() const { return node(id::Server); }

  status_promise<scada::node> add_node(const AddNodesItem& item);

  status_promise<void> acknowledge_events(std::vector<EventId> event_ids,
                                          DateTime acknowledge_time) const;

  status_promise<void> acknowledge_event(EventId event_id,
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
