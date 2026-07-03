#pragma once

#include "metrics/trace_id.h"
#include "scada/node_id.h"

#include <memory>
#include <ostream>

namespace scada {

class [[nodiscard]] ServiceContext {
 public:
  ServiceContext() = default;

  ServiceContext(const ServiceContext&) = default;
  ServiceContext& operator=(const ServiceContext&) = default;

  const scada::NodeId& user_id() const;
  // The caller's access-rights bitmask (bits from scada::Privilege), captured at
  // session activation. Zero for an anonymous or unauthenticated context.
  uint32_t user_rights() const;
  // True when there is no authenticated user (an anonymous session): a null
  // user_id.
  bool is_anonymous() const;
  uint64_t request_id() const;
  const TraceId& trace_id() const;

  ServiceContext with_user_id(const scada::NodeId& user_id) const;
  ServiceContext with_user_rights(uint32_t user_rights) const;
  ServiceContext with_request_id(uint64_t request_id) const;
  ServiceContext with_trace_id(const TraceId& trace_id) const;

  friend std::ostream& operator<<(std::ostream& stream,
                                  const ServiceContext& context);

 private:
  struct Rep;

  explicit ServiceContext(const std::shared_ptr<const Rep>& rep);

  std::shared_ptr<const Rep> rep_ = kDefaultRep;

  static const std::shared_ptr<const Rep> kDefaultRep;
};

}  // namespace scada
