#include "scada/extension_object.h"

#include <gmock/gmock.h>

namespace scada {

struct TestObject {};

TEST(ExtensionObject, ConstructExtract) {
  const ExpandedNodeId kTestTypeId{123, "uri", 22};
  TestObject test_object;
  ExtensionObject extension_object{kTestTypeId, std::move(test_object)};
  EXPECT_EQ(kTestTypeId, extension_object.data_type_id());
  auto* saved_object = std::any_cast<TestObject>(&extension_object.value());
  ASSERT_TRUE(saved_object);
}

namespace {

struct Payload {
  int value = 0;
};

// A stand-in for the bridge's encoder: Payload's int as its bytes.
std::optional<ByteString> EncodePayload(const ExtensionObject& object) {
  const auto* payload = std::any_cast<Payload>(&object.value());
  if (!payload)
    return std::nullopt;
  const auto* bytes = reinterpret_cast<const char*>(&payload->value);
  return ByteString{bytes, bytes + sizeof payload->value};
}

// Installs EncodePayload for one test, and removes it after.
class ScopedEncoder {
 public:
  ScopedEncoder() { SetExtensionObjectEncoder(&EncodePayload); }
  ~ScopedEncoder() { SetExtensionObjectEncoder(nullptr); }
};

const ExpandedNodeId kPayloadTypeId{NodeId{7u, 2}};

}  // namespace

// Equality used to be false always, even for an object and itself.
TEST(ExtensionObject, EqualPayloadsCompareEqual) {
  ScopedEncoder encoder;
  const ExtensionObject a{kPayloadTypeId, Payload{5}};
  EXPECT_EQ(a, a);
  EXPECT_EQ(a, (ExtensionObject{kPayloadTypeId, Payload{5}}));
  EXPECT_NE(a, (ExtensionObject{kPayloadTypeId, Payload{6}}));
  EXPECT_NE(a, (ExtensionObject{ExpandedNodeId{NodeId{8u, 2}}, Payload{5}}));
}

// A payload the encoder cannot encode, or any payload without an encoder,
// stays unequal; two payload-less objects of one type are equal.
TEST(ExtensionObject, UnencodablePayloadsStayUnequal) {
  const ExtensionObject a{kPayloadTypeId, Payload{5}};
  EXPECT_NE(a, a);  // no encoder installed

  ScopedEncoder encoder;
  const ExtensionObject other{kPayloadTypeId, TestObject{}};
  EXPECT_NE(other, other);

  EXPECT_EQ((ExtensionObject{kPayloadTypeId, {}}),
            (ExtensionObject{kPayloadTypeId, {}}));
  EXPECT_NE((ExtensionObject{kPayloadTypeId, {}}), a);
}

}  // namespace scada
