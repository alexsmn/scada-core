#pragma once

#include "base/lifetime.h"
#include "metrics/trace_id.h"
#include "scada/node_id.h"

#include <memory>
#include <ostream>
#include <string>
#include <vector>

namespace scada {

class [[nodiscard]] ServiceContext {
 public:
  ServiceContext() = default;

  ServiceContext(const ServiceContext&) = default;
  ServiceContext& operator=(const ServiceContext&) = default;

  const scada::NodeId& user_id() const SCADA_LIFETIME_BOUND;
  // The caller's access-rights bitmask (bits from scada::AccessRight), captured
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
  // The session's preferred locales, most preferred first, as the client
  // supplied them at session activation (RFC 3066 ids such as "en" or
  // "ru-RU"). Empty when the client named none, which Part 4 §5.4 leaves the
  // server free to answer in any locale it has. Consume it through
  // `scada::SelectLocalizedText` (scada/locale_negotiation.h) rather than by
  // reading element 0 — the list is a preference order, not a single choice.
  // OPC UA Part 4 §5.4 Locale Negotiation,
  // https://reference.opcfoundation.org/Core/Part4/v105/docs/5.4
  const std::vector<std::string>& locale_ids() const SCADA_LIFETIME_BOUND;

  ServiceContext with_user_id(const scada::NodeId& user_id) const;
  ServiceContext with_user_rights(uint32_t user_rights) const;
  ServiceContext with_request_id(uint64_t request_id) const;
  ServiceContext with_trace_id(const TraceId& trace_id) const;
  ServiceContext with_peer(std::string peer) const;
  ServiceContext with_locale_ids(std::vector<std::string> locale_ids) const;

  friend std::ostream& operator<<(std::ostream& stream,
                                  const ServiceContext& context);

 private:
  struct Rep;

  explicit ServiceContext(const std::shared_ptr<const Rep>& rep);

  std::shared_ptr<const Rep> rep_ = kDefaultRep;

  static const std::shared_ptr<const Rep> kDefaultRep;
};

}  // namespace scada
