#pragma once

#include "scada/node_id.h"
#include "scada/standard_node_ids.h"

#include <optional>

namespace scada {

// The standard OPC UA ReferenceType hierarchy (namespace 0) is fixed by the
// specification and identical on every server, so its subtype relationships can
// be answered statically — without the ReferenceType nodes being present in a
// (possibly remote) address space. Custom, server-defined reference types
// (namespace != 0) are NOT covered here: their ids are server-relative and must
// be resolved against the owning server's address space.
//
// OPC UA Part 3 §7 ReferenceType hierarchy,
// https://reference.opcfoundation.org/Core/Part3/v105/docs/7 .

// The immediate supertype of a standard reference type within namespace 0, or 0
// if `numeric_id` is the hierarchy root (References) or is not a recognized
// standard reference type.
constexpr NumericId StandardReferenceTypeSupertype(NumericId numeric_id) {
  switch (numeric_id) {
    case id::HierarchicalReferences:
    case id::NonHierarchicalReferences:
      return id::References;

    case id::HasChild:
    case id::Organizes:
    case id::HasEventSource:
      return id::HierarchicalReferences;

    case id::Aggregates:
    case id::HasSubtype:
      return id::HasChild;

    case id::HasComponent:
    case id::HasProperty:
    case id::HasHistoricalConfiguration:
      return id::Aggregates;

    // HasOrderedComponent (i=49) has no named constant; it is a subtype of
    // HasComponent.
    case 49:
      return id::HasComponent;

    case id::HasNotifier:
      return id::HasEventSource;

    case id::HasModellingRule:
    case id::HasTypeDefinition:
      return id::NonHierarchicalReferences;

    default:
      // References (the root) and any non-standard reference type.
      return 0;
  }
}

// Whether `sub_type_id` is `super_type_id` or one of its subtypes within the
// standard reference-type hierarchy. Returns nullopt when the relationship is
// not statically knowable — i.e. `sub_type_id` is not a recognized standard
// reference type (custom namespace, or a standard id not modelled above) — in
// which case the caller must fall back to walking the address space.
//
// A recognized (standard) `sub_type_id` always yields a definitive answer,
// including `false` when `super_type_id` is a custom reference type: standard
// reference types never derive from custom ones.
inline std::optional<bool> IsStandardReferenceSubtype(
    const NodeId& sub_type_id,
    const NodeId& super_type_id) {
  if (sub_type_id.namespace_index() != 0 || !sub_type_id.is_numeric())
    return std::nullopt;

  NumericId current = sub_type_id.numeric_id();
  const bool recognized =
      current == id::References || StandardReferenceTypeSupertype(current) != 0;
  if (!recognized)
    return std::nullopt;

  const bool super_is_standard =
      super_type_id.namespace_index() == 0 && super_type_id.is_numeric();
  const NumericId super_numeric_id =
      super_is_standard ? super_type_id.numeric_id() : 0;

  // Walk up the fixed supertype chain. `recognized` guarantees every non-root
  // link resolves, so `current` reaches References and the loop terminates.
  for (;;) {
    if (super_is_standard && current == super_numeric_id)
      return true;
    if (current == id::References)
      return false;
    current = StandardReferenceTypeSupertype(current);
  }
}

}  // namespace scada
