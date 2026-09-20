#pragma once

#include "scada/localized_text.h"
#include "scada/string.h"

#include <span>
#include <string>
#include <string_view>
#include <vector>

namespace scada {

// Locale negotiation: picking which translation of a LocalizedText a session
// gets. OPC UA Part 4 §5.4 Locale Negotiation,
// https://reference.opcfoundation.org/Core/Part4/v105/docs/5.4
//
// A session carries an ordered array of LocaleIds (most preferred first),
// supplied by the client. The server "shall return the translation which is
// the most preferred that it can"; failing that, any translation it has; and
// when the client named no locale at all, again any one it has. Nothing here
// ever returns an empty result while the server holds a translation — an
// unmatched request degrades to a different language, never to no text.

// The special locale ids defined by OPC UA Part 3 §8.5.2,
// https://reference.opcfoundation.org/Core/Part3/v105/docs/8.5 — "mul" packs
// several languages into one LocalizedText and "qst" adds consumer
// substitutions, both encoding the payload as a JSON object rather than plain
// text. Part 3 §8.5.2 forbids either as a server's default locale.
inline constexpr std::string_view kMultiLanguageLocale = "mul";
inline constexpr std::string_view kSubstitutableLocale = "qst";

// True for "mul" and "qst" (case-insensitive), the two locale ids whose text
// payload is a JSON object instead of a plain string.
bool IsSpecialLocale(std::string_view locale_id);

// True when `available` satisfies a request for `requested`, comparing RFC
// 3066 tags case-insensitively: either the whole tags are equal, or their
// primary language subtags are ("en-US" is served by "en", and "en" by
// "en-US"). This is the lookup rule the selection below applies at each step
// of the client's preference list; it is a deliberate simplification of RFC
// 4647 lookup, which would instead truncate the requested tag one subtag at a
// time. The difference only shows on tags with three or more subtags, which
// no configuration in this tree uses.
bool LocaleMatches(std::string_view available, std::string_view requested);

// Packs several translations into one LocalizedText in the "mul" form: locale
// "mul", text a minified JSON object `{"t":[["ru","…"],["en","…"]]}` whose
// pairs are locale-then-text. OPC UA Part 3 §8.5.2.2 Multiple language locale,
// https://reference.opcfoundation.org/Core/Part3/v105/docs/8.5
//
// Returns the single entry unchanged when `translations` holds exactly one, so
// the JSON form only appears where it buys something. Entries are emitted in
// the order given, which is the order a later `SelectLocalizedText` falls back
// through.
LocalizedText EncodeMultiLanguage(std::span<const LocalizedText> translations);

// The inverse of `EncodeMultiLanguage`, and the only way to get translations
// back out of a value that crossed a wire as one LocalizedText.
//
// A "mul" value is unpacked into its pairs; a "qst" value is unpacked from the
// same "t" key, with its "r" substitutions left unapplied (this tree neither
// emits nor consumes them). Anything else — including a "mul" whose text does
// not parse — comes back as a single-element vector holding the input, so a
// caller never has to distinguish the packed from the plain case. An empty
// input yields an empty vector.
std::vector<LocalizedText> DecodeMultiLanguage(const LocalizedText& text);

// Picks the translation a session gets, per Part 4 §5.4.
//
// `translations` is everything the server holds for a single LocalizedText,
// each entry normally carrying a non-empty `locale`; `requested` is the
// session's LocaleIds, most preferred first. Returns the chosen entry
// unchanged (so its `locale` travels to the client), or an empty
// LocalizedText when `translations` is empty.
//
// A leading "mul" or "qst" in the request is honoured as Part 4 §5.4
// describes: alone it asks for every language the server has, and followed by
// locales it narrows to those. The answer is then a "mul" value — never a
// "qst" one, which this server does not produce, and which §5.4 allows ("If a
// Client requests 'qst' it shall be prepared to receive a 'qst', 'mul' or
// another locale"). A special locale anywhere but the front is ignored.
LocalizedText SelectLocalizedText(std::span<const LocalizedText> translations,
                                  std::span<const String> requested);

// Resolves one stored LocalizedText — which may or may not be a packed "mul"
// value — against a session's preferences: `DecodeMultiLanguage` then
// `SelectLocalizedText`. This is the call site shape almost everything has,
// and it carries its own name rather than overloading `SelectLocalizedText`
// because `LocalizedText`'s implicit constructors make an overload pair
// ambiguous for a braced argument.
LocalizedText ResolveLocalizedText(const LocalizedText& stored,
                                   std::span<const String> requested);

}  // namespace scada
