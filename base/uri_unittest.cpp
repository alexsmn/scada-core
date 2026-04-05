#include "base/uri.h"

#include <algorithm>
#include <format>
#include <gmock/gmock.h>
#include <string>

namespace net {
namespace {

struct EscapeCase {
  const char* input;
  const char* output;
};

TEST(EscapeTest, EscapeTextForFormSubmission) {
  const EscapeCase escape_cases[] = {
      {"foo", "foo"}, {"foo bar", "foo+bar"}, {"foo++", "foo%2B%2B"}};
  for (const auto& escape_case : escape_cases) {
    EXPECT_EQ(escape_case.output,
              EscapeQueryParamValue(escape_case.input, true));
  }

  const EscapeCase escape_cases_no_plus[] = {
      {"foo", "foo"}, {"foo bar", "foo%20bar"}, {"foo++", "foo%2B%2B"}};
  for (const auto& escape_case : escape_cases_no_plus) {
    EXPECT_EQ(escape_case.output,
              EscapeQueryParamValue(escape_case.input, false));
  }

  // Test all the values in we're supposed to be escaping.
  const std::string no_escape(
      "abcdefghijklmnopqrstuvwxyz"
      "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
      "0123456789"
      "!'()*-._~");
  for (int i = 0; i < 256; ++i) {
    std::string in;
    in.push_back(static_cast<char>(i));
    std::string out = EscapeQueryParamValue(in, true);
    if (0 == i) {
      EXPECT_EQ(out, std::string("%00"));
    } else if (32 == i) {
      // Spaces are plus escaped like web forms.
      EXPECT_EQ(out, std::string("+"));
    } else if (no_escape.find(in) == std::string::npos) {
      // Check %hex escaping
      std::string expected = std::format("%{:02X}", i);
      EXPECT_EQ(expected, out);
    } else {
      // No change for things in the no_escape list.
      EXPECT_EQ(out, in);
    }
  }
}

TEST(EscapeTest, EscapePath) {
  ASSERT_EQ(
      // Most of the character space we care about, un-escaped
      EscapePath("\x02\n\x1d !\"#$%&'()*+,-./0123456789:;"
                 "<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                 "[\\]^_`abcdefghijklmnopqrstuvwxyz"
                 "{|}~\x7f\x80\xff"),
      // Escaped
      "%02%0A%1D%20!%22%23$%25&'()*+,-./0123456789%3A;"
      "%3C=%3E%3F@ABCDEFGHIJKLMNOPQRSTUVWXYZ"
      "%5B%5C%5D%5E_%60abcdefghijklmnopqrstuvwxyz"
      "%7B%7C%7D~%7F%80%FF");
}

}  // namespace
}  // namespace net
