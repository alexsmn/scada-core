#pragma once

#include "base/lifetime.h"
#include "metrics/trace_id.h"
#include "scada/node_id.h"

#include <memory>
#include <ostream>
#include <string>

namespace scada {

class [[nodiscard]] ServiceContext {
 public:
  ServiceContext() = default;

  ServiceContext(const ServiceContext&) = default;
  ServiceContext& operator=(const ServiceContext&) = default;

  const scada::NodeId& user_id() const SCADA_LIFETIME_BOUND;
  // The caller's access-rights bitmask (bits from scada::Privilege), captured
  // at session activation. Zero for an anonymous or unauthenticated context.
  uint32_t user_rights() const;
  // True when there is no authenticated user (an anonymous session): a null
  // user_id.
  bool is_anonymous() const;
  uint64_t request_id() const;
  const TraceId& trace_id() const SCADA_LIFETIME_BOUND;
  // Remote network peer of the caller's connection ("address:port"), captured
  // by the serving transport at session activation. Empty when unknown (e.g.
  // an in-process caller). The OTel `client.address` equivalent for request
  // logs and trace spans.
  const std::string& peer() const SCADA_LIFETIME_BOUND;

  ServiceContext with_user_id(const scada::NodeId& user_id) const;
  ServiceContext with_user_rights(uint32_t user_rights) const;
  ServiceContext with_request_id(uint64_t request_id) const;
  ServiceContext with_trace_id(const TraceId& trace_id) const;
  ServiceContext with_peer(std::string peer) const;

  friend std::ostream& operator<<(std::ostream& stream,
                                  const ServiceContext& context);

 private:
  struct Rep;

  explicit ServiceContext(const std::shared_ptr<const Rep>& rep);

  std::shared_ptr<const Rep> rep_ = kDefaultRep;

  static const std::shared_ptr<const Rep> kDefaultRep;
};

}  // namespace scada
