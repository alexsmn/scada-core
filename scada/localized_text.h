#pragma once

#include "base/lifetime.h"
#include "scada/string.h"

#include <concepts>
#include <string>
#include <string_view>

#define LOCALIZED_TEXT(text) L##text

namespace scada {

// Human-readable text with an associated locale id. Mirrors the OPC UA
// built-in LocalizedText: `locale` is an RFC 3066 identifier such as "en" or
// "ru" (empty when unspecified), `text` is the UTF-16 payload. OPC UA Part 3
// §8.5 LocalizedText,
// https://reference.opcfoundation.org/Core/Part3/v105/docs/8.5
//
// Implicitly constructible from `std::u16string` / `char16_t*` and keeps a
// minimal string-ish surface (empty/clear/append/operator+=/comparisons) so
// text-only call sites read like the plain string the type used to be.
struct LocalizedText {
  LocalizedText() = default;
  // Implicit by design: the type replaced a bare `std::u16string` typedef and
  // most call sites still hand it plain UTF-16 strings.
  // NOLINTNEXTLINE(google-explicit-constructor)
  LocalizedText(std::u16string text) : text{std::move(text)} {}
  // NOLINTNEXTLINE(google-explicit-constructor)
  LocalizedText(std::u16string_view text) : text{text} {}
  // NOLINTNEXTLINE(google-explicit-constructor)
  LocalizedText(const char16_t* text) : text{text ? text : u""} {}
  LocalizedText(String locale, std::u16string text)
      : locale{std::move(locale)}, text{std::move(text)} {}

  // A LocalizedText is null/empty only when both fields are empty (a
  // locale-only value still round-trips through the codecs).
  bool empty() const noexcept { return locale.empty() && text.empty(); }
  void clear() noexcept {
    locale.clear();
    text.clear();
  }

  // Text-only appends; the locale of the left-hand side is retained. The
  // constrained template binds anything u16string-view-like (literals,
  // std::u16string, views) by exact reference match, which keeps overload
  // resolution unambiguous against the LocalizedText overload below.
  template <class S>
    requires std::convertible_to<const S&, std::u16string_view>
  LocalizedText& append(const S& suffix) {
    text.append(std::u16string_view{suffix});
    return *this;
  }
  template <class S>
    requires std::convertible_to<const S&, std::u16string_view>
  LocalizedText& operator+=(const S& suffix) {
    text.append(std::u16string_view{suffix});
    return *this;
  }
  LocalizedText& operator+=(char16_t ch) {
    text += ch;
    return *this;
  }
  LocalizedText& operator+=(const LocalizedText& other) {
    text.append(other.text);
    return *this;
  }

  // Compares both fields; the implicit constructors make comparisons against
  // `u"..."` literals compile (those compare with an empty locale).
  friend bool operator==(const LocalizedText&, const LocalizedText&) = default;
  friend auto operator<=>(const LocalizedText&, const LocalizedText&) = default;

  String locale;
  std::u16string text;
};

// Conversion from a scada::String which is a UTF-8 string.
LocalizedText ToLocalizedText(std::string_view string);

inline LocalizedText ToLocalizedText(std::u16string_view string) {
  return LocalizedText{string};
}

inline LocalizedText ToLocalizedText(const std::u16string& string) {
  return LocalizedText{string};
}

inline LocalizedText ToLocalizedText(std::u16string&& string) {
  return LocalizedText{std::move(string)};
}

}  // namespace scada

// Builds a debug string which is a native MB string. It's not neccessary a
// UTF-8 string like scada::String.
std::string ToString(const scada::LocalizedText& text);

inline const std::u16string& ToString16(
    const scada::LocalizedText& localized_text SCADA_LIFETIME_BOUND) {
  return localized_text.text;
}
