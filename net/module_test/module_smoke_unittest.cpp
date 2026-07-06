// Smoke test for the scada.net module facade: names come from
// `import scada.net;` only, including base names re-exported through
// `export import scada.base;`.

#include <memory>
#include <type_traits>

// transport:: names are third-party and not exported by the facade; TUs
// using them include the transport headers textually alongside the import.
#include <transport/log.h>

#include <gtest/gtest.h>

// Import after the textual includes (AppleClang 21 libc++ merging bug).
import scada.net;

namespace scada_net_module {
namespace {

TEST(ScadaNetModuleSmoke, LoggerAdapter) {
  auto logger = std::make_shared<NullLogger>();  // via scada.base re-export
  NetLoggerAdapter adapter{logger};
  adapter.Write(transport::LogSeverity::Normal, "smoke");
}

TEST(ScadaNetModuleSmoke, ExecutorAdapterAlias) {
  static_assert(std::is_same_v<NetExecutorAdapter, AnyExecutor>);
}

}  // namespace
}  // namespace scada_net_module
