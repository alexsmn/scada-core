#include "scada/node_id_log.h"

#include <format>

namespace scada {

namespace {

// Set once at startup by the model layer and read thereafter; a function-local
// static keeps it out of static-global-init ordering (matching the project's
// no-static-globals rule and `base::PathService`).
NamespaceNameResolver& GetNamespaceNameResolver() {
  static NamespaceNameResolver resolver = nullptr;
  return resolver;
}

}  // namespace

void SetNamespaceNameResolver(NamespaceNameResolver resolver) {
  GetNamespaceNameResolver() = resolver;
}

std::string NodeIdToLogString(const NodeId& node_id) {
  std::string namespace_name;
  if (const NamespaceNameResolver resolver = GetNamespaceNameResolver())
    namespace_name = std::string{resolver(node_id.namespace_index())};
  if (namespace_name.empty())
    namespace_name = std::format("NS{}", node_id.namespace_index());

  switch (node_id.type()) {
    case NodeIdType::Numeric:
      return std::format("{}.{}", namespace_name, node_id.numeric_id());

    case NodeIdType::String:
      return std::format("{}.{}", namespace_name,
                         std::string{node_id.string_id()});

    default:
      // Opaque identifiers have no log-string form; callers treat an empty
      // result as unsupported.
      return {};
  }
}

}  // namespace scada
