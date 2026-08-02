#include "scada/variant.h"
#include "scada/standard_node_ids.h"
#include "scada/variant_utils.h"

#include <gmock/gmock.h>
#include <sstream>

using namespace testing;

// `ToString(Variant::Type)` and `ToNodeId(Variant::Type)` are two tables
// indexed by the same enum, and only one of them had a test. "INT64" and
// "UINT64" were transposed in the name table, so every diagnostic that printed
// a Variant's type named the wrong one — self-consistent under the
// ToString/ParseBuiltInType round trip, and therefore invisible until someone
// read a failure message and disbelieved it.
//
// Anchored to the OPC UA built-in type ids (Part 6 §5.1.2 Built-in Types,
// https://reference.opcfoundation.org/Core/Part6/v105/docs/5.1.2), which is the
// independent fact both tables encode, rather than to each other.
TEST(VariantTest, TypeNameAndNodeIdAgreeOnEveryType) {
  const auto name_of = [](scada::Variant::Type type) { return ToString(type); };

  EXPECT_EQ("INT32", name_of(scada::Variant::INT32));
  EXPECT_EQ("UINT32", name_of(scada::Variant::UINT32));
  EXPECT_EQ("INT64", name_of(scada::Variant::INT64));
  EXPECT_EQ("UINT64", name_of(scada::Variant::UINT64));

  EXPECT_EQ(scada::id::Int64,
            scada::ToNodeId(scada::Variant::INT64).numeric_id());
  EXPECT_EQ(scada::id::UInt64,
            scada::ToNodeId(scada::Variant::UINT64).numeric_id());

  // The two directions must compose: a live value's type, named and resolved
  // back to a DataType node id, is the id a nodeset would declare for it.
  EXPECT_EQ(scada::ToBuiltInDataType(scada::ToNodeId(scada::Variant::UINT64)),
            scada::Variant::UINT64);
  EXPECT_EQ(scada::Variant{scada::UInt64{1}}.type(), scada::Variant::UINT64);
  EXPECT_EQ(scada::Variant{scada::Int64{1}}.type(), scada::Variant::INT64);
}

TEST(VariantTest, ToString16) {
  EXPECT_EQ(scada::Variant::kFalseString, ToString16(scada::Variant{false}));
  EXPECT_EQ(scada::Variant::kTrueString, ToString16(scada::Variant{true}));
  EXPECT_EQ(u"Привет", ToString16(scada::Variant{u"Привет"}));
}

// ConvertVariant<std::wstring>

TEST(VariantTest, ConvertToWstringFromLocalizedText) {
  scada::Variant v{u"hello"};
  std::wstring result;
  EXPECT_TRUE(scada::ConvertVariant(v, result));
  EXPECT_EQ(L"hello", result);
}

TEST(VariantTest, ConvertToWstringFromLocalizedTextNonAscii) {
  scada::Variant v{u"\u041F\u0440\u0438\u0432\u0435\u0442"};  // "Привет"
  std::wstring result;
  EXPECT_TRUE(scada::ConvertVariant(v, result));
  EXPECT_EQ(L"\x041F\x0440\x0438\x0432\x0435\x0442", result);
}

TEST(VariantTest, ConvertToWstringFromEmptyVariant) {
  // EMPTY variant converts to an empty wstring (ToStringHelper clears target).
  scada::Variant v;
  std::wstring result;
  EXPECT_TRUE(scada::ConvertVariant(v, result));
  EXPECT_TRUE(result.empty());
}

TEST(VariantTest, ConvertToWstringFromInt) {
  // Integer variants are formatted as decimal strings via ToStringHelper.
  scada::Variant v{scada::Int32{42}};
  std::wstring result;
  EXPECT_TRUE(scada::ConvertVariant(v, result));
  EXPECT_EQ(L"42", result);
}

// Variant::Dump must be safe for every alternative — including LocalizedText
// (which is a std::u16string and has no std::ostream::operator<<). A missing
// specialisation in DumpHelper used to either fail to compile or, under
// MSVC's permissive lookup, pick an overload that silently looped, causing
// GTest's pretty-printer for vector<DataValue> to hang. Pin behaviour with a
// stream-roundtrip test for each alternative we care about.
TEST(VariantTest, DumpsLocalizedTextAsUtf8WithoutHanging) {
  scada::Variant v{scada::LocalizedText{u"Pump"}};
  std::ostringstream stream;
  v.Dump(stream);
  EXPECT_EQ(stream.str(), R"("Pump")");
}

TEST(VariantTest, DumpsEmptyVariantAsNull) {
  scada::Variant v;
  std::ostringstream stream;
  v.Dump(stream);
  EXPECT_EQ(stream.str(), "null");
}

TEST(VariantTest, DumpsScalarsThroughOstreamOperator) {
  // The free `operator<<(std::ostream&, const Variant&)` defined in the
  // header is the entry point GTest uses when pretty-printing failing
  // EXPECT_EQ assertions. Sanity-check it for the alternatives Phase 0+
  // exercises so a future regression in DumpHelper surfaces here directly.
  std::ostringstream s_int;
  s_int << scada::Variant{scada::Int32{42}};
  EXPECT_EQ(s_int.str(), "42");

  std::ostringstream s_double;
  s_double << scada::Variant{2.5};
  EXPECT_EQ(s_double.str(), "2.5");

  std::ostringstream s_text;
  s_text << scada::Variant{scada::LocalizedText{u"Pump"}};
  EXPECT_EQ(s_text.str(), R"("Pump")");

  std::ostringstream s_string;
  s_string << scada::Variant{std::string{"hello"}};
  EXPECT_EQ(s_string.str(), "hello");
}
