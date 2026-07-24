#pragma once

#include "base/awaitable.h"
#include "scada/status_or.h"

namespace scada {

// Coroutine-result shorthands for the two shapes that dominate the async
// service APIs: an operation reporting only success/failure, and one returning
// a value or a failure. `CoStatus` is `Awaitable<Status>`; `CoStatusOr<T>` is
// `Awaitable<StatusOr<T>>`. They exist purely to shorten signatures — the
// spelled-out forms remain valid and identical in meaning.
using CoStatus = Awaitable<Status>;

template <class T>
using CoStatusOr = Awaitable<StatusOr<T>>;

}  // namespace scada
