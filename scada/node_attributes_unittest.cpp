#include "scada/node_attributes.h"

#include "scada/standard_node_ids.h"

#include <gtest/gtest.h>

namespace scada {
namespace {

TEST(NodeAttributesTest, SetStoresValueAttribute) {
  // Regression: `Set` assigned its own parameter (which shadows the member)
  // instead of the attribute, so writing Value silently did nothing.
  NodeAttributes attributes;
  ASSERT_FALSE(attributes.value.has_value());

  EXPECT_EQ(attributes.Set(AttributeId::Value, Variant{42}),
            StatusCode::Good);

  ASSERT_TRUE(attributes.value.has_value());
  EXPECT_EQ(*attributes.value, Variant{42});
  EXPECT_EQ(attributes.Get(AttributeId::Value), Variant{42});
  EXPECT_FALSE(attributes.empty());
}

TEST(NodeAttributesTest, SetOverwritesAnExistingValue) {
  NodeAttributes attributes{.value = Variant{1}};

  EXPECT_EQ(attributes.Set(AttributeId::Value, Variant{2}), StatusCode::Good);

  EXPECT_EQ(attributes.Get(AttributeId::Value), Variant{2});
}

TEST(NodeAttributesTest, SetRoundTripsEverySupportedAttribute) {
  NodeAttributes attributes;

  EXPECT_EQ(attributes.Set(AttributeId::BrowseName, QualifiedName{"Flow", 2}),
            StatusCode::Good);
  EXPECT_EQ(attributes.Set(AttributeId::DisplayName, LocalizedText{u"Flow"}),
            StatusCode::Good);
  EXPECT_EQ(attributes.Set(AttributeId::InverseName, LocalizedText{u"FlowOf"}),
            StatusCode::Good);
  EXPECT_EQ(attributes.Set(AttributeId::DataType, NodeId{id::UInt32}),
            StatusCode::Good);
  EXPECT_EQ(attributes.Set(AttributeId::Value, Variant{7}), StatusCode::Good);

  EXPECT_EQ(attributes.Get(AttributeId::BrowseName),
            Variant{QualifiedName("Flow", 2)});
  EXPECT_EQ(attributes.Get(AttributeId::DisplayName),
            Variant{LocalizedText(u"Flow")});
  EXPECT_EQ(attributes.Get(AttributeId::InverseName),
            Variant{LocalizedText(u"FlowOf")});
  EXPECT_EQ(attributes.Get(AttributeId::DataType), Variant{NodeId(id::UInt32)});
  EXPECT_EQ(attributes.Get(AttributeId::Value), Variant{7});
}

TEST(NodeAttributesTest, SetRejectsMistypedAndUnsupportedAttributes) {
  NodeAttributes attributes;

  // A non-Value attribute whose variant carries the wrong type is refused, and
  // the attribute is left untouched.
  EXPECT_EQ(attributes.Set(AttributeId::BrowseName, Variant{42}),
            StatusCode::Bad);
  EXPECT_TRUE(attributes.browse_name.empty());

  EXPECT_EQ(attributes.Set(AttributeId::NodeId, Variant{42}),
            StatusCode::Bad_WrongAttributeId);
  EXPECT_TRUE(attributes.empty());
}

}  // namespace
}  // namespace scada
