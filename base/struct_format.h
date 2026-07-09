#pragma once

#include "base/debug_util.h"
#include "base/lifetime.h"

#include <format>
#include <span>
#include <string>
#include <string_view>

// std::format-native counterpart to StructWriter. It renders a struct as
// `{name: value, ...}` directly into a std::format output iterator, with no
// std::ostream / std::ostringstream indirection. It is meant to be used from
// inside a std::formatter<T>::format() body:
//
//   template <>
//   struct std::formatter<MyType> {
//     constexpr auto parse(std::format_parse_context& ctx) {
//       auto it = ctx.begin();
//       if (it != ctx.end() && *it != '}')
//         throw std::format_error{"MyType takes no format spec"};
//       return it;
//     }
//     template <class FormatContext>
//     auto format(const MyType& v, FormatContext& ctx) const {
//       return StructFormatter{ctx.out()}
//           .AddField("a", v.a)
//           .AddField("b", v.b)
//           .Finish();
//     }
//   };
//
// Differences from StructWriter worth noting when comparing the two shapes:
//   * The closing brace is emitted by Finish(), which returns the advanced
//     output iterator. StructWriter relied on its destructor to write `}`,
//     but a formatter has to *return* the final iterator, so RAII closing is
//     not an option here.
//   * Field values are rendered through std::format, so nested struct types
//     must be std::formattable (e.g. via their own std::formatter). This is
//     the bottom-up enablement the migration away from operator<< requires.
//   * String values are quoted *and escaped*, and bool renders as
//     true/false — behavior centralized here rather than left to operator<<.
// Mixin supplying a std::formatter parse() that accepts only the empty "{}"
// format spec and rejects any non-empty spec with std::format_error. Derive a
// std::formatter from this when the type renders the same regardless of spec:
//
//   template <>
//   struct std::formatter<MyType> : EmptyFormatSpec {
//     template <class FormatContext>
//     auto format(const MyType& v, FormatContext& ctx) const { ... }
//   };
struct EmptyFormatSpec {
  constexpr std::format_parse_context::iterator parse(
      std::format_parse_context& ctx) const {
    const std::format_parse_context::iterator it = ctx.begin();
    if (it != ctx.end() && *it != '}')
      throw std::format_error{"type takes no format spec"};
    return it;
  }
};

template <class OutputIt>
class StructFormatter {
 public:
  explicit StructFormatter(OutputIt out) : out_{out} { *out_++ = '{'; }

  // Adds a `name: value` field. `value` may be any std::formattable type;
  // strings are quoted/escaped and bool renders as true/false.
  template <class T>
  StructFormatter& AddField(std::string_view name,
                            const T& value) SCADA_LIFETIME_BOUND {
    BeginField(name);
    AppendValue(value);
    return *this;
  }

  // Adds a `name: ["Bit0", "Bit2"]` field listing the set bits' labels.
  StructFormatter& AddBitMaskField(
      std::string_view name,
      unsigned bit_mask,
      std::span<const std::string_view> bit_strings) SCADA_LIFETIME_BOUND {
    BeginField(name);
    out_ = std::format_to(out_, "{}", BitMaskToString(bit_mask, bit_strings));
    return *this;
  }

  // Emits the closing brace and returns the final output iterator for the
  // enclosing formatter to return.
  OutputIt Finish() {
    *out_++ = '}';
    return out_;
  }

 private:
  void BeginField(std::string_view name) {
    if (count_++ != 0)
      out_ = std::format_to(out_, ", ");
    out_ = std::format_to(out_, "{}: ", name);
  }

  // Generic path: any std::formattable value.
  template <class T>
  void AppendValue(const T& value) {
    out_ = std::format_to(out_, "{}", value);
  }

  void AppendValue(bool value) {
    out_ = std::format_to(out_, "{}", value ? "true" : "false");
  }

  void AppendValue(std::string_view value) { AppendQuoted(value); }
  void AppendValue(const std::string& value) { AppendQuoted(value); }
  void AppendValue(const char* value) { AppendQuoted(value); }

  void AppendQuoted(std::string_view value) {
    *out_++ = '"';
    for (const char c : value) {
      switch (c) {
        case '"':
          out_ = std::format_to(out_, "\\\"");
          break;
        case '\\':
          out_ = std::format_to(out_, "\\\\");
          break;
        case '\n':
          out_ = std::format_to(out_, "\\n");
          break;
        case '\r':
          out_ = std::format_to(out_, "\\r");
          break;
        case '\t':
          out_ = std::format_to(out_, "\\t");
          break;
        default:
          *out_++ = c;
          break;
      }
    }
    *out_++ = '"';
  }

  OutputIt out_;
  std::size_t count_ = 0;
};

template <class OutputIt>
StructFormatter(OutputIt) -> StructFormatter<OutputIt>;
