#pragma once

// Transitional compatibility shims for the server-wide `X::` -> `scada::X::`
// namespace migration. Every server-owned module namespace now nests under
// `scada`; these using-directives let existing unqualified call sites keep
// resolving while migrated incrementally. Remove once consumers use `scada::`.
//
// Physically homed in core/base (scada_base) rather than server/base so it is
// reachable from EVERY module through the `base/` include root that scada_base
// already PUBLIC-exports — including the lean server framework seams (e.g.
// scada_server_history_db_sync) that link scada_base transitively but not
// server_base. The shimmed namespaces are server-owned, but the header is
// include-only: nothing in core/common includes it, so these declarations stay
// confined to the server TUs that do.
namespace scada::aggregation {}
namespace aggregation {
using namespace scada::aggregation;  // NOLINT(build/namespaces) transitional shim
}
namespace scada::configuration {}
namespace configuration {
using namespace scada::configuration;  // NOLINT(build/namespaces) transitional shim
}
namespace scada::data_items {}
namespace data_items {
using namespace scada::data_items;  // NOLINT(build/namespaces) transitional shim
}
namespace scada::database_node_manager3 {}
namespace database_node_manager3 {
using namespace scada::database_node_manager3;  // NOLINT(build/namespaces) transitional shim
}
namespace scada::devices {}
namespace devices {
using namespace scada::devices;  // NOLINT(build/namespaces) transitional shim
}
namespace scada::discovery {}
namespace discovery {
using namespace scada::discovery;  // NOLINT(build/namespaces) transitional shim
}
namespace scada::driver {}
namespace driver {
using namespace scada::driver;  // NOLINT(build/namespaces) transitional shim
}
namespace scada::events {}
namespace events {
using namespace scada::events;  // NOLINT(build/namespaces) transitional shim
}
namespace scada::filesystem {}
namespace filesystem {
using namespace scada::filesystem;  // NOLINT(build/namespaces) transitional shim
}
namespace scada::history {}
namespace history {
using namespace scada::history;  // NOLINT(build/namespaces) transitional shim
}
namespace scada::license {}
namespace license {
using namespace scada::license;  // NOLINT(build/namespaces) transitional shim
}
namespace scada::opc {}
namespace opc {
using namespace scada::opc;  // NOLINT(build/namespaces) transitional shim
}
namespace scada::redundancy {}
namespace redundancy {
using namespace scada::redundancy;  // NOLINT(build/namespaces) transitional shim
}
namespace scada::replication {}
namespace replication {
using namespace scada::replication;  // NOLINT(build/namespaces) transitional shim
}
namespace scada::security {}
namespace security {
using namespace scada::security;  // NOLINT(build/namespaces) transitional shim
}
namespace scada::server {}
namespace server {
using namespace scada::server;  // NOLINT(build/namespaces) transitional shim
}
namespace scada::simulator {}
namespace simulator {
using namespace scada::simulator;  // NOLINT(build/namespaces) transitional shim
}
namespace scada::vidicon {}
namespace vidicon {
using namespace scada::vidicon;  // NOLINT(build/namespaces) transitional shim
}
