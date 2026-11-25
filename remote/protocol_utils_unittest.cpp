#include <gmock/gmock.h>

#include "remote/protocol_utils.h"
#include "scada/attribute_ids.h"
#include "scada/authorization.h"
#include "scada/event.h"
#include "scada/extension_object.h"
#include "scada/identity_mapping_rule_encoding.h"
#include "scada/user_management_encoding.h"
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

// A UserManagementDataType payload (one entry of the UserManagement object's
// Users property, OPC UA Part 18 §5.2.4) survives the gRPC boundary. The whole
// users grid is one Read of this array, so a payload that degraded to its type
// id here would leave the client with a list of nameless accounts.
TEST(ProtocolUtils, ExtensionObjectUserManagementUserRoundTrip) {
  const scada::UserManagementDataType user{
      .user_name = "ivanov",
      .user_configuration = scada::UserConfiguration::kDisabled |
                            scada::UserConfiguration::kMustChangePassword,
      .description = "Dispatcher, north"};
  const auto original = scada::EncodeUserManagementUsers(std::vector{user});

  protocol::Variant proto;
  Convert(original, proto);
  const auto restored = ConvertTo<scada::Variant>(proto);

  const auto decoded = scada::DecodeUserManagementUsers(restored);
  ASSERT_TRUE(decoded.has_value());
  ASSERT_EQ(decoded->size(), 1u);
  EXPECT_EQ(decoded->front(), user);
}

// An account carrying no configuration bits is a real answer -- an ordinary,
// enabled user -- and must survive as such rather than arriving as an absent
// payload.
TEST(ProtocolUtils, ExtensionObjectUserManagementDefaultsRoundTrip) {
  const scada::UserManagementDataType user{.user_name = "root"};
  const auto original = scada::EncodeUserManagementUsers(std::vector{user});

  protocol::Variant proto;
  Convert(original, proto);
  const auto decoded =
      scada::DecodeUserManagementUsers(ConvertTo<scada::Variant>(proto));

  ASSERT_TRUE(decoded.has_value());
  ASSERT_EQ(decoded->size(), 1u);
  EXPECT_EQ(decoded->front().user_name, "root");
  EXPECT_EQ(decoded->front().user_configuration,
            scada::UserConfiguration::kNone);
  EXPECT_TRUE(decoded->front().description.empty());
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

TEST(ProtocolUtils, OpaqueNodeIdRoundTripsAsOpaque) {
  const scada::NodeId node_id{scada::ByteString{'\x01', '\x02', '\x00'}, 2};
  protocol::NodeId proto_node_id;
  Convert(node_id, proto_node_id);
  auto restored_node_id = ConvertTo<scada::NodeId>(proto_node_id);
  EXPECT_EQ(restored_node_id.type(), scada::NodeIdType::Opaque);
  EXPECT_EQ(node_id, restored_node_id);
}

TEST(ProtocolUtils, GuidNodeIdRoundTripsAsGuid) {
  const scada::NodeId node_id{
      *scada::Guid::FromString("C496578A-0DFE-4B8F-870A-745238C6AEAE"), 3};
  protocol::NodeId proto_node_id;
  Convert(node_id, proto_node_id);
  EXPECT_EQ(proto_node_id.guid_id().size(), 16u);
  auto restored_node_id = ConvertTo<scada::NodeId>(proto_node_id);
  EXPECT_EQ(restored_node_id.type(), scada::NodeIdType::Guid);
  EXPECT_EQ(node_id, restored_node_id);
}

// Wire data never panics: a guid_id of the wrong length reads as null.
TEST(ProtocolUtils, MalformedGuidNodeIdIsNull) {
  protocol::NodeId proto_node_id;
  proto_node_id.set_namespace_index(3);
  proto_node_id.set_guid_id("short");
  EXPECT_TRUE(ConvertTo<scada::NodeId>(proto_node_id).is_null());
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
  source.time = scada::Now();
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
  source.time = scada::Now();
  source.severity = 0;

  protocol::Event proto;
  Convert(source, proto);
  EXPECT_TRUE(proto.message_utf8().empty());
}

// A Variant type this wire cannot carry degrades to null on encode; it used
// to hit NotReached() and abort the server answering the read.
TEST(ProtocolUtils, UnencodableVariantEncodesAsNull) {
  const scada::Variant expanded{scada::ExpandedNodeId{scada::NodeId{5u, 2}}};
  EXPECT_TRUE(ConvertTo<scada::Variant>(ConvertTo<protocol::Variant>(expanded))
                  .is_null());

  const scada::Variant node_ids{
      std::vector<scada::NodeId>{scada::NodeId{1u, 2}, scada::NodeId{2u, 2}}};
  EXPECT_TRUE(ConvertTo<scada::Variant>(ConvertTo<protocol::Variant>(node_ids))
                  .is_null());
}

// A peer's sub_code at or above 0x4000 used to spill into the severity bits,
// so a Good code arrived Bad.
TEST(ProtocolUtils, StatusCodeSubCodeCannotChangeTheSeverity) {
  protocol::StatusCode proto;
  proto.set_severity(protocol::Good);
  proto.set_sub_code(0x8000u);
  EXPECT_TRUE(scada::IsGood(ConvertTo<scada::StatusCode>(proto)));
}

// Re-encoding what a peer sent must not abort: an event with id 0 and an
// invalid model-change event both used to fail a Check.
TEST(ProtocolUtils, PeerSuppliedEventsEncodeWithoutAborting) {
  scada::Event event;
  event.event_id = 0;
  EXPECT_EQ(ConvertTo<protocol::Event>(event).event_id(), 0u);

  scada::ModelChangeEvent model_change;  // no node, no verb: invalid
  EXPECT_FALSE(
      ConvertTo<protocol::ModelChangeEvent>(model_change).node_added());
}

// A target on another server cannot be expressed on this wire; it is left
// out instead of failing a Check and aborting.
TEST(ProtocolUtils, BrowsePathResultLeavesOutNonLocalTargets) {
  scada::BrowsePathResult result;
  result.targets.push_back(
      {.target_id = scada::ExpandedNodeId{scada::NodeId{1u, 2}}});
  result.targets.push_back({.target_id = scada::ExpandedNodeId{
                                scada::NodeId{2u, 2}, "urn:other", 1}});
  const auto proto = ConvertTo<protocol::BrowsePathResult>(result);
  ASSERT_EQ(proto.target_size(), 1);
  EXPECT_EQ(ConvertTo<scada::NodeId>(proto.target(0).target_id()),
            scada::NodeId(1u, 2));
}

// OPC UA defines attribute ids past the end of the name table; naming one
// used to read out of bounds.
TEST(ProtocolUtils, UnknownAttributeIdHasAName) {
  EXPECT_EQ(ToString(static_cast<scada::AttributeId>(27)), "AttributeId(27)");
  EXPECT_EQ(ToString(scada::AttributeId::Value), "Value");
}
