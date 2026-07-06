// Smoke test for the scada.remote module facade: names come from
// `import scada.remote;` only, including scada.core / scada.base names
// re-exported through `export import`.

#include <string>

#include <gtest/gtest.h>

// Import after the textual includes (AppleClang 21 libc++ merging bug).
import scada.remote;

namespace scada_remote_module {
namespace {

TEST(ScadaRemoteModuleSmoke, ProtocolFramingHelpers) {
  std::string message;
  protocol::PrependMessageSize(message);
  protocol::AppendMessage(message, "abc", 3);
  protocol::UpdateMessageSize(message);
  EXPECT_EQ(protocol::GetMessagePayloadSize(message), 3u);
}

TEST(ScadaRemoteModuleSmoke, ExportedTypesAndTransitiveSurface) {
  SubscriptionParams params{};
  MonitoredItemParams item_params{};
  (void)params;
  (void)item_params;

  // Transitive scada.core / scada.base surfaces via export import.
  scada::NodeId node_id{5, 1};
  EXPECT_EQ(ToString(scada::StatusCode::Good), "Good");
  base::Check(!node_id.is_null(), "remote module smoke");
}

}  // namespace
}  // namespace scada_remote_module
