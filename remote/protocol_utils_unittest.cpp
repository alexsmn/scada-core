#include <gmock/gmock.h>

#include "remote/protocol_utils.h"
#include "scada/authorization.h"
#include "scada/event.h"
#include "scada/extension_object.h"
#include "scada/identity_mapping_rule_encoding.h"
#include "scada/variant.h"

#include <any>
#include <vector>

TEST(ProtocolUtils, ExtensionObjectRolePermissionArrayRoundTrip) {
  const scada::RolePermissionType role{
      .role_id = scada::NodeId{15680u, 0},  // WellKnownRole_Operator
      .permissions = scada::Permission::kRead | scada::Permission::kWrite};
  std::vector<scada::ExtensionObject> objects;
  objects.emplace_back(scada::ExpandedNodeId{scada::NodeId{96u, 0}},
                       std::any{role});
  const scada::Variant original{std::move(objects)};

  protocol::Variant proto;
  Convert(original, proto);
  const auto restored = ConvertTo<scada::Variant>(proto);

  ASSERT_TRUE(restored.is_array());
  ASSERT_EQ(restored.type(), scada::Variant::EXTENSION_OBJECT);
  const auto& restored_objects =
      restored.get<std::vector<scada::ExtensionObject>>();
  ASSERT_EQ(restored_objects.size(), 1u);
  EXPECT_EQ(restored_objects.front().data_type_id().node_id(),
            (scada::NodeId{96u, 0}));

  const auto* decoded = std::any_cast<scada::RolePermissionType>(
      &restored_objects.front().value());
  ASSERT_NE(decoded, nullptr);
  EXPECT_EQ(decoded->role_id, (scada::NodeId{15680u, 0}));
  EXPECT_EQ(decoded->permissions,
            (scada::Permission::kRead | scada::Permission::kWrite));
}

// An IdentityMappingRuleType payload (a Role's Identities entry or an
// AddIdentity/RemoveIdentity argument, OPC UA Part 18 §4.4.3) survives the
// gRPC boundary.
TEST(ProtocolUtils, ExtensionObjectIdentityMappingRuleRoundTrip) {
  const scada::IdentityMappingRule rule{
      .criteria_type = scada::IdentityCriteriaType::kUserName,
      .criteria = "svc-site-a"};
  std::vector<scada::ExtensionObject> objects;
  objects.emplace_back(scada::ExpandedNodeId{scada::NodeId{
                           scada::kIdentityMappingRuleTypeDataTypeId, 0}},
                       std::any{rule});
  const scada::Variant original{std::move(objects)};

  protocol::Variant proto;
  Convert(original, proto);
  const auto restored = ConvertTo<scada::Variant>(proto);

  ASSERT_TRUE(restored.is_array());
  ASSERT_EQ(restored.type(), scada::Variant::EXTENSION_OBJECT);
  const auto& restored_objects =
      restored.get<std::vector<scada::ExtensionObject>>();
  ASSERT_EQ(restored_objects.size(), 1u);

  const auto decoded =
      scada::DecodeIdentityMappingRule(restored_objects.front());
  ASSERT_TRUE(decoded.has_value());
  EXPECT_EQ(*decoded, rule);
}

TEST(ProtocolUtils, NodeId) {
  // A nested (String-type) NodeId; the exact namespace is immaterial to this
  // protobuf round-trip test. Constructed directly to keep core/remote free of
  // the model layer (HISTORICAL_DB == namespace index 6).
  scada::NodeId node_id{scada::String{"4!PendingTaskCount"}, 6};
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
  std::string original = "\xd0\x9f\xd1\x80\xd0\xb8\xd0\xb2\xd0\xb5\xd1\x82";
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
  source.time = scada::base::NowUtc();
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
  source.time = scada::base::NowUtc();
  source.severity = 0;

  protocol::Event proto;
  Convert(source, proto);
  EXPECT_TRUE(proto.message_utf8().empty());
}
