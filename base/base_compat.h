#pragma once

// Transitional compatibility shim for the `base::` -> `scada::base::` migration.
// All of `core/base/` now declares its symbols in the nested `scada::base`
// namespace. This using-directive lets existing unqualified `base::` call sites
// (in core and the common/server/client consumers) keep resolving while they
// are migrated incrementally. Remove this header and its includes once every
// consumer has been rewritten to `scada::base::`.
namespace scada::base {}
namespace base {
using namespace scada::base;  // NOLINT(build/namespaces) transitional shim
}  // namespace base
