// Hybrid-mode test for the scada.remote module facade: textual includes of
// the protobuf-facing headers (which the facade deliberately excludes)
// compose with the import in one TU - the documented pattern for
// pb-touching consumers.

#include "remote/protocol.h"
#include "remote/protocol_utils.h"

#include <gtest/gtest.h>

// Import after the textual includes (AppleClang 21 libc++ merging bug).
import scada.remote;

namespace scada_remote_module {
namespace {

TEST(ScadaRemoteModuleMixed, ProtobufHeadersComposeWithImport) {
  // pb message type from the textual include...
  protocol::NodeId pb_node_id;
  pb_node_id.set_numeric_id(42);

  // ...converted via the Convert overloads from protocol_utils.h (textual),
  // into the scada types that arrive through the import.
  scada::NodeId node_id;
  Convert(pb_node_id, node_id);
  EXPECT_EQ(node_id.numeric_id(), 42u);
  EXPECT_EQ(node_id, scada::NodeId(42, 0));
}

}  // namespace
}  // namespace scada_remote_module
