#pragma once

#include "scada/node_id.h"

#include <string>
#include <string_view>

namespace scada {

// Maps a namespace index to its short name (e.g. 7 -> "SCADA"). Returns an empty
// view for unknown indices. The name table lives in the model layer, which is
// layered above `scada_core`; this seam lets low-level code (e.g. the remote
// protocol) render friendly namespace prefixes without a compile-time
// dependency on the model. Mirrors `base::PathService` provider registration.
using NamespaceNameResolver = std::string_view (*)(NamespaceIndex);

// Installs the resolver used by `NodeIdToLogString`. The model layer installs
// the SCADA namespace table at startup (see the model's resolver registration).
// Without it, `NodeIdToLogString` falls back to the "NS<n>" numeric prefix.
void SetNamespaceNameResolver(NamespaceNameResolver resolver);

// Renders a NodeId in the human-friendly "<NsName>.<id>" form used in logs
// (e.g. "SCADA.123", "USER.5"). Falls back to "NS<n>" when no resolver is
// installed or the namespace is unknown. Opaque identifiers have no textual
// form and yield an empty string. This is a logging aid; it deliberately does
// not know the model's nested-alias vocabulary (that stays in
// `NodeIdToScadaString` in the model layer).
std::string NodeIdToLogString(const NodeId& node_id);

}  // namespace scada
