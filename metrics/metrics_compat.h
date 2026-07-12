#pragma once

// Transitional compatibility shim for the `metrics::` -> `scada::metrics::`
// migration. All of `core/metrics/` now declares its symbols in the nested
// `scada::metrics` namespace; this using-directive lets existing unqualified
// `metrics::` call sites keep resolving while they are migrated incrementally.
// Remove this header and its includes once every consumer has been rewritten to
// `scada::metrics::`.
namespace scada::metrics {}
namespace metrics {
using namespace scada::metrics;  // NOLINT(build/namespaces) transitional shim
}  // namespace metrics
