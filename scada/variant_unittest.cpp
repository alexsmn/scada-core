#include "scada/variant.h"
#include "scada/variant_utils.h"

#include <gmock/gmock.h>
#include <sstream>

using namespace testing;

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
