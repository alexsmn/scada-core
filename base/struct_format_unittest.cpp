#include "base/struct_format.h"

#include <format>
#include <string>
#include <string_view>

#include <gtest/gtest.h>

namespace {

// A leaf type formatted directly via std::format (no operator<<, no
// std::ostream). This is the target shape for migrated types.
struct Inner {
  int code = 0;
};

// A composite type that nests a formattable sub-struct, exercises string
// escaping, bool rendering, and a bitmask field.
struct Outer {
  std::string name;
  bool active = false;
  unsigned flags = 0;
  Inner inner;
};

// Shared "no format spec" parse used by the test formatters.
struct NoSpecParse {
  constexpr std::format_parse_context::iterator parse(
      std::format_parse_context& ctx) const {
    const std::format_parse_context::iterator it = ctx.begin();
    if (it != ctx.end() && *it != '}')
      throw std::format_error{"type takes no format spec"};
    return it;
  }
};

}  // namespace

template <>
struct std::formatter<Inner> : NoSpecParse {
  template <class FormatContext>
  auto format(const Inner& v, FormatContext& ctx) const {
    return StructFormatter{ctx.out()}.AddField("code", v.code).Finish();
  }
};

template <>
struct std::formatter<Outer> : NoSpecParse {
  template <class FormatContext>
  auto format(const Outer& v, FormatContext& ctx) const {
    static constexpr std::string_view kFlagStrings[] = {"A", "B", "C"};
    return StructFormatter{ctx.out()}
        .AddField("name", v.name)
        .AddField("active", v.active)
        .AddBitMaskField("flags", v.flags, kFlagStrings)
        .AddField("inner", v.inner)
        .Finish();
  }
};

namespace {

TEST(StructFormatterTest, RendersNestedStruct) {
  const Outer v{.name = "node", .active = true, .flags = 0b101, .inner = {7}};
  EXPECT_EQ(
      std::format("{}", v),
      "{name: \"node\", active: true, flags: [\"A\",\"C\"], inner: {code: 7}}");
}

TEST(StructFormatterTest, RendersBoolAsWord) {
  const Outer v{.name = "x", .active = false, .flags = 0, .inner = {0}};
  EXPECT_EQ(std::format("{}", v),
            "{name: \"x\", active: false, flags: [], inner: {code: 0}}");
}

TEST(StructFormatterTest, EscapesStringValues) {
  const Inner inner;
  const Outer v{
      .name = "a\"b\\c\nd", .active = false, .flags = 0, .inner = inner};
  EXPECT_EQ(std::format("{}", v),
            "{name: \"a\\\"b\\\\c\\nd\", active: false, flags: [], "
            "inner: {code: 0}}");
}

TEST(StructFormatterTest, EmbedsInLargerFormatString) {
  const Inner v{42};
  EXPECT_EQ(std::format("inner={}!", v), "inner={code: 42}!");
}

}  // namespace
