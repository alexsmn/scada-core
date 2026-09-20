#include "scada/locale_negotiation.h"

#include "base/utf_convert.h"

#include <boost/json.hpp>

#include <algorithm>
#include <cctype>

namespace scada {

namespace {

char AsciiLower(char ch) {
  return static_cast<char>(
      std::tolower(static_cast<unsigned char>(ch)));
}

bool EqualsIgnoreCase(std::string_view left, std::string_view right) {
  return std::ranges::equal(left, right, [](char a, char b) {
    return AsciiLower(a) == AsciiLower(b);
  });
}

// The primary language subtag of an RFC 3066 tag: everything before the first
// '-' ("en" for "en-US", "ru" for "ru").
std::string_view PrimarySubtag(std::string_view locale_id) {
  const auto separator = locale_id.find('-');
  return separator == std::string_view::npos ? locale_id
                                             : locale_id.substr(0, separator);
}

}  // namespace

bool IsSpecialLocale(std::string_view locale_id) {
  return EqualsIgnoreCase(locale_id, kMultiLanguageLocale) ||
         EqualsIgnoreCase(locale_id, kSubstitutableLocale);
}

bool LocaleMatches(std::string_view available, std::string_view requested) {
  if (available.empty() || requested.empty())
    return false;
  return EqualsIgnoreCase(available, requested) ||
         EqualsIgnoreCase(PrimarySubtag(available), PrimarySubtag(requested));
}

LocalizedText EncodeMultiLanguage(
    std::span<const LocalizedText> translations) {
  if (translations.empty())
    return LocalizedText{};
  if (translations.size() == 1)
    return translations.front();

  boost::json::array pairs;
  pairs.reserve(translations.size());
  for (const LocalizedText& translation : translations) {
    pairs.emplace_back(boost::json::array{
        boost::json::string{translation.locale},
        boost::json::string{UtfConvert<char>(translation.text)}});
  }
  // `serialize` emits the minified form Part 3 §8.5.2.2 asks for, and escapes
  // the text itself, so nothing here has to think about quotes or surrogates.
  const boost::json::object object{{"t", std::move(pairs)}};
  return LocalizedText{String{kMultiLanguageLocale},
                       UtfConvert<char16_t>(boost::json::serialize(object))};
}

std::vector<LocalizedText> DecodeMultiLanguage(const LocalizedText& text) {
  if (text.empty())
    return {};
  if (!IsSpecialLocale(text.locale))
    return {text};

  boost::system::error_code error;
  const boost::json::value parsed =
      boost::json::parse(UtfConvert<char>(text.text), error);
  if (error || !parsed.is_object())
    return {text};
  const boost::json::value* pairs = parsed.as_object().if_contains("t");
  if (!pairs || !pairs->is_array())
    return {text};

  std::vector<LocalizedText> translations;
  for (const boost::json::value& pair : pairs->as_array()) {
    if (!pair.is_array() || pair.as_array().size() < 2)
      continue;
    const boost::json::value& locale = pair.as_array()[0];
    const boost::json::value& value = pair.as_array()[1];
    if (!locale.is_string() || !value.is_string())
      continue;
    translations.emplace_back(
        String{locale.as_string().c_str()},
        UtfConvert<char16_t>(std::string_view{value.as_string()}));
  }
  // A packed value that carried no usable pair is more useful to the caller as
  // itself than as nothing — the text is still displayable, if ugly.
  if (translations.empty())
    return {text};
  return translations;
}

LocalizedText SelectLocalizedText(std::span<const LocalizedText> translations,
                                  std::span<const String> requested) {
  if (translations.empty())
    return LocalizedText{};

  // Part 4 §5.4: a special locale is only meaningful "as the first entry of
  // the list". Alone it asks for every language; followed by locales it
  // narrows to those, and asking for languages the server does not have still
  // has to answer something, so an empty narrowing degrades to everything.
  if (!requested.empty() && IsSpecialLocale(requested.front())) {
    const std::span<const String> wanted = requested.subspan(1);
    if (wanted.empty())
      return EncodeMultiLanguage(translations);
    std::vector<LocalizedText> narrowed;
    for (const String& locale : wanted) {
      for (const LocalizedText& candidate : translations) {
        if (LocaleMatches(candidate.locale, locale))
          narrowed.push_back(candidate);
      }
    }
    return narrowed.empty() ? EncodeMultiLanguage(translations)
                            : EncodeMultiLanguage(narrowed);
  }

  // Walk the client's preferences in order and take the first translation any
  // of them matches. An exact tag wins over a primary-subtag match within the
  // same preference step, so a session asking for "en-GB" prefers an "en-GB"
  // translation to an "en-US" one even though both are acceptable.
  for (const String& wanted : requested) {
    if (IsSpecialLocale(wanted))
      continue;
    const LocalizedText* subtag_match = nullptr;
    for (const LocalizedText& candidate : translations) {
      if (EqualsIgnoreCase(candidate.locale, wanted))
        return candidate;
      if (!subtag_match && LocaleMatches(candidate.locale, wanted))
        subtag_match = &candidate;
    }
    if (subtag_match)
      return *subtag_match;
  }

  // Nothing the client asked for is available (or it asked for nothing): Part
  // 4 §5.4 says to return one the server does have.
  return translations.front();
}

LocalizedText ResolveLocalizedText(const LocalizedText& stored,
                                   std::span<const String> requested) {
  const std::vector<LocalizedText> translations = DecodeMultiLanguage(stored);
  return SelectLocalizedText(translations, requested);
}

}  // namespace scada
