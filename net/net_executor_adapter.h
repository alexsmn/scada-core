#pragma once

#include "base/any_executor.h"

namespace scada {
using NetExecutorAdapter = AnyExecutor;
}  // namespace scada

// Transitional compatibility shim: expose the historically global-scope name
// until all callers migrate to `scada::`.
using scada::NetExecutorAdapter;  // NOLINT(build/namespaces) transitional
