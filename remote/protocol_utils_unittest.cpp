#include <gmock/gmock.h>

#include "model/node_id_util.h"
#include "remote/protocol_utils.h"
#include "scada/event.h"

TEST(ProtocolUtils, NodeId) {
  auto node_id = NodeIdFromScadaString("HISTORICAL_DB.4!PendingTaskCount");
  protocol::NodeId proto_node_id;
  Convert(node_id, proto_node_id);
  auto restored_node_id = ConvertTo<scada::NodeId>(proto_node_id);
  EXPECT_EQ(node_id, restored_node_id);
}

// LocalizedText conversions

TEST(ProtocolUtils, ConvertStringToLocalizedTextAscii) {
  scada::LocalizedText target;
  Convert(std::string("hello"), target);
  EXPECT_EQ(u"hello", target);
}

TEST(ProtocolUtils, ConvertStringToLocalizedTextUtf8) {
  scada::LocalizedText target;
  // "Привет" in UTF-8
  Convert(std::string("\xd0\x9f\xd1\x80\xd0\xb8\xd0\xb2\xd0\xb5\xd1\x82"),
          target);
  EXPECT_EQ(u"\u041F\u0440\u0438\u0432\u0435\u0442", target);
}

TEST(ProtocolUtils, ConvertLocalizedTextToString) {
  std::string target;
  Convert(scada::LocalizedText(u"hello"), target);
  EXPECT_EQ("hello", target);
}

TEST(ProtocolUtils, ConvertLocalizedTextRoundTrip) {
  std::string original = "test string";
  scada::LocalizedText lt;
  Convert(original, lt);
  std::string restored;
  Convert(lt, restored);
  EXPECT_EQ(original, restored);
}

TEST(ProtocolUtils, ConvertLocalizedTextRoundTripUtf8) {
  // "Привет" in UTF-8
  std::string original =
      "\xd0\x9f\xd1\x80\xd0\xb8\xd0\xb2\xd0\xb5\xd1\x82";
  scada::LocalizedText lt;
  Convert(original, lt);
  std::string restored;
  Convert(lt, restored);
  EXPECT_EQ(original, restored);
}

// Event conversions

TEST(ProtocolUtils, ConvertEventPreservesMessage) {
  scada::Event source;
  source.event_id = 1;
  source.time = base::Time::Now();
  source.severity = 100;
  source.message = u"Test event message";

  protocol::Event proto;
  Convert(source, proto);
  EXPECT_EQ("Test event message", proto.message_utf8());

  scada::Event restored;
  Convert(proto, restored);
  EXPECT_EQ(source.message, restored.message);
}

TEST(ProtocolUtils, ConvertEventEmptyMessage) {
  scada::Event source;
  source.event_id = 2;
  source.time = base::Time::Now();
  source.severity = 0;

  protocol::Event proto;
  Convert(source, proto);
  EXPECT_TRUE(proto.message_utf8().empty());
}
