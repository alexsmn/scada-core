// scada.net — named C++20 module facade over the core/net headers.
//
// Same design and rules as scada.base (see base/scada_base.cppm and
// docs/cxx-modules.md): headers stay the source of truth, the global module
// fragment includes them, the purview re-exports names with `export using`.
// `export import scada.base;` mirrors scada_net's PUBLIC link on scada_base.

module;

// ---- Global module fragment: headers stay the source of truth ----
#include "net/net_boost_logger_adapter.h"
#include "net/net_executor_adapter.h"

export module scada.net;

export import scada.base;

export {
  // net_boost_logger_adapter.h / net_executor_adapter.h
  using ::NetBoostLoggerAdapter;
  using ::NetExecutorAdapter;
}  // export
