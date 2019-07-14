#include <gmock/gmock.h>

#include "model/node_id_util.h"
#include "remote/protocol_utils.h"

TEST(ProtocolUtils, NodeId) {
  auto node_id = NodeIdFromScadaString("HISTORICAL_DB.4!PendingTaskCount");
  protocol::NodeId proto_node_id;
  Convert(node_id, proto_node_id);
  auto restored_node_id = Convert<scada::NodeId>(proto_node_id);
  EXPECT_EQ(node_id, restored_node_id);
}
