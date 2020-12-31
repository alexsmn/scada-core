#include <gmock/gmock.h>

#include "core/node_id.h"
#include "model/namespaces.h"
#include "model/node_id_util.h"

TEST(NodeIdUtil, NodeIdFromScadaString) {
  EXPECT_EQ(scada::NodeId(53, NamespaceIndexes::TS),
            NodeIdFromScadaString("TS.53"));
  EXPECT_EQ(scada::NodeId(15, NamespaceIndexes::IEC60870_DEVICE),
            NodeIdFromScadaString("IEC_DEV.15"));
  EXPECT_EQ(scada::NodeId(12, NamespaceIndexes::IEC60870_DEVICE),
            NodeIdFromScadaString("T11.12"));
  EXPECT_EQ(scada::NodeId("MODBUS_PORTS.32!Online", 0),
            NodeIdFromScadaString("MODBUS_PORTS.32!Online"));
  EXPECT_EQ(scada::NodeId("MODBUS_PORTS.32!BIT:4", 0),
            NodeIdFromScadaString("MODBUS_PORTS.32!BIT:4"));
}

TEST(NodeIdUtil, NodeIdToScadaString) {
  EXPECT_EQ(NodeIdToScadaString(scada::NodeId(53, NamespaceIndexes::TS)),
            "TS.53");
  EXPECT_EQ(
      NodeIdToScadaString(scada::NodeId(15, NamespaceIndexes::IEC60870_DEVICE)),
            "IEC_DEV.15");
  EXPECT_EQ(NodeIdToScadaString(scada::NodeId("MODBUS_PORTS.32!Online", 0)),
            "MODBUS_PORTS.32!Online");
  EXPECT_EQ(NodeIdToScadaString(scada::NodeId("MODBUS_PORTS.32!BIT:4", 0)),
            "MODBUS_PORTS.32!BIT:4");
}

TEST(NodeIdUtil, Comparison) {
  EXPECT_LT(NodeIdFromScadaString("HISTORICAL_DB.1!WriteEventDuration"),
            NodeIdFromScadaString("HISTORICAL_DB.1!WriteValueDuration"));

  EXPECT_LT(NodeIdFromScadaString("HISTORICAL_DB.1!WriteEventDuration"),
            NodeIdFromScadaString("HISTORICAL_DB.3!WriteEventDuration"));

  EXPECT_EQ(NodeIdFromScadaString("HISTORICAL_DB.1!WriteEventDuration"),
            NodeIdFromScadaString("HISTORICAL_DB.1!WriteEventDuration"));
}

TEST(NodeIdUtil, IsNestedNodeId) {
  scada::NodeId kNodeId{"IEC_DEV.1!1", 0};
  scada::NodeId parent_id;
  std::string_view nested_name;
  EXPECT_TRUE(IsNestedNodeId(kNodeId, parent_id, nested_name));
  EXPECT_EQ(NodeIdFromScadaString("IEC_DEV.1"), parent_id);
  EXPECT_EQ("1", nested_name);
}
