#include "scada/locale_negotiation.h"

#include <gtest/gtest.h>

#include <span>
#include <vector>

namespace scada {
namespace {

LocalizedText Ru() {
  return LocalizedText{"ru", u"Напряжение"};
}
LocalizedText En() {
  return LocalizedText{"en", u"Voltage"};
}
LocalizedText EnGb() {
  return LocalizedText{"en-GB", u"Voltage (GB)"};
}

TEST(LocaleNegotiationTest, NoTranslationsYieldsEmpty) {
  EXPECT_TRUE(SelectLocalizedText({}, {}).empty());
}

TEST(LocaleNegotiationTest, NoRequestedLocaleYieldsFirstAvailable) {
  // Part 4 §5.4: "If the Client fails to specify at least one LocaleId, the
  // Server shall return any one that it has."
  const std::vector<LocalizedText> translations{Ru(), En()};
  EXPECT_EQ(Ru(), SelectLocalizedText(translations, {}));
}

TEST(LocaleNegotiationTest, ExactMatchWins) {
  const std::vector<LocalizedText> translations{Ru(), En()};
  const std::vector<String> requested{"en"};
  EXPECT_EQ(En(), SelectLocalizedText(translations, requested));
}

TEST(LocaleNegotiationTest, PreferenceOrderIsHonoured) {
  const std::vector<LocalizedText> translations{Ru(), En()};
  const std::vector<String> first_en{"en", "ru"};
  const std::vector<String> first_ru{"ru", "en"};
  EXPECT_EQ(En(), SelectLocalizedText(translations, first_en));
  EXPECT_EQ(Ru(), SelectLocalizedText(translations, first_ru));
}

TEST(LocaleNegotiationTest, RegionalRequestMatchesPlainLanguage) {
  const std::vector<LocalizedText> translations{Ru(), En()};
  const std::vector<String> requested{"en-US"};
  EXPECT_EQ(En(), SelectLocalizedText(translations, requested));
}

TEST(LocaleNegotiationTest, PlainRequestMatchesRegionalTranslation) {
  const std::vector<LocalizedText> translations{Ru(), EnGb()};
  const std::vector<String> requested{"en"};
  EXPECT_EQ(EnGb(), SelectLocalizedText(translations, requested));
}

TEST(LocaleNegotiationTest, ExactTagBeatsSubtagMatchAtTheSameStep) {
  const std::vector<LocalizedText> translations{EnGb(), En()};
  const std::vector<String> requested{"en"};
  EXPECT_EQ(En(), SelectLocalizedText(translations, requested));
}

TEST(LocaleNegotiationTest, MatchingIsCaseInsensitive) {
  const std::vector<LocalizedText> translations{Ru(), En()};
  const std::vector<String> requested{"EN"};
  EXPECT_EQ(En(), SelectLocalizedText(translations, requested));
}

TEST(LocaleNegotiationTest, UnmatchedRequestFallsBackToAnAvailableLocale) {
  // Part 4 §5.4: "If it does not have a translation for any of the locales
  // identified in this list, then it shall return LocalizedText in an
  // available locale." Never nothing.
  const std::vector<LocalizedText> translations{Ru(), En()};
  const std::vector<String> requested{"de", "fr"};
  EXPECT_EQ(Ru(), SelectLocalizedText(translations, requested));
}

TEST(LocaleNegotiationTest, SpecialLocaleOffTheFrontIsIgnored) {
  // Part 4 §5.4 gives "mul"/"qst" a meaning only "as the first entry of the
  // list"; elsewhere it is a locale nothing has, so the next entry decides.
  const std::vector<LocalizedText> translations{Ru(), En()};
  const std::vector<String> requested{"de", "mul", "en"};
  EXPECT_EQ(En(), SelectLocalizedText(translations, requested));
}

TEST(LocaleNegotiationTest, LeadingMulAloneReturnsEveryLanguage) {
  // Part 4 §5.4: "If there are no further entries, the Server shall return
  // all languages available."
  const std::vector<LocalizedText> translations{Ru(), En()};
  const std::vector<String> requested{"mul"};
  const LocalizedText selected = SelectLocalizedText(translations, requested);
  EXPECT_EQ(String{"mul"}, selected.locale);
  EXPECT_EQ(translations, DecodeMultiLanguage(selected));
}

TEST(LocaleNegotiationTest, LeadingMulWithLocalesNarrowsToThem) {
  // Part 4 §5.4: "If there are more languages included after 'mul' […] the
  // Server shall return only those languages from that list."
  const std::vector<LocalizedText> translations{Ru(), En(), EnGb()};
  const std::vector<String> requested{"mul", "ru"};
  const LocalizedText selected = SelectLocalizedText(translations, requested);
  EXPECT_EQ(std::vector<LocalizedText>{Ru()}, DecodeMultiLanguage(selected));
}

TEST(LocaleNegotiationTest, LeadingMulNarrowedToNothingStillAnswers) {
  // §5.4 again: with no translation for any locale in the list the server
  // "shall return LocalizeText in an available locale" — so everything it has
  // beats returning nothing.
  const std::vector<LocalizedText> translations{Ru(), En()};
  const std::vector<String> requested{"mul", "ja"};
  EXPECT_EQ(translations,
            DecodeMultiLanguage(SelectLocalizedText(translations, requested)));
}

TEST(LocaleNegotiationTest, QstIsAnsweredWithMulNeverWithQst) {
  // §5.4: "If a Client requests 'qst' it shall be prepared to receive a
  // 'qst', 'mul' or another locale". This server has no substitutions to
  // offer, so it answers the multi-language form.
  const std::vector<LocalizedText> translations{Ru(), En()};
  const std::vector<String> requested{"qst"};
  EXPECT_EQ(String{"mul"},
            SelectLocalizedText(translations, requested).locale);
}

TEST(LocaleNegotiationTest, MultiLanguageRoundTrips) {
  const std::vector<LocalizedText> translations{Ru(), En()};
  const LocalizedText packed = EncodeMultiLanguage(translations);
  EXPECT_EQ(String{"mul"}, packed.locale);
  EXPECT_EQ(translations, DecodeMultiLanguage(packed));
}

TEST(LocaleNegotiationTest, MultiLanguageUsesTheSpecShape) {
  // Part 3 §8.5.2.2: a JSON object with a "t" key holding [locale, text]
  // pairs, minified.
  const std::vector<LocalizedText> translations{En(),
                                                LocalizedText{"de", u"Spannung"}};
  EXPECT_EQ(u"{\"t\":[[\"en\",\"Voltage\"],[\"de\",\"Spannung\"]]}",
            EncodeMultiLanguage(translations).text);
}

TEST(LocaleNegotiationTest, MultiLanguageOfOneStaysPlain) {
  // Packing buys nothing for a single translation, and a plain value is what
  // every existing consumer already understands.
  const std::vector<LocalizedText> translations{En()};
  EXPECT_EQ(En(), EncodeMultiLanguage(translations));
  EXPECT_TRUE(EncodeMultiLanguage({}).empty());
}

TEST(LocaleNegotiationTest, DecodingAPlainValueYieldsItself) {
  EXPECT_EQ(std::vector<LocalizedText>{En()}, DecodeMultiLanguage(En()));
  EXPECT_TRUE(DecodeMultiLanguage(LocalizedText{}).empty());
}

TEST(LocaleNegotiationTest, DecodingMalformedMulYieldsTheValueItself) {
  // Never nothing: the raw text is ugly but displayable, and dropping it
  // would turn a malformed translation into a missing node label.
  const LocalizedText broken{"mul", u"not json at all"};
  EXPECT_EQ(std::vector<LocalizedText>{broken}, DecodeMultiLanguage(broken));
  const LocalizedText no_pairs{"mul", u"{\"t\":[]}"};
  EXPECT_EQ(std::vector<LocalizedText>{no_pairs}, DecodeMultiLanguage(no_pairs));
}

TEST(LocaleNegotiationTest, QstPayloadIsUnpackedFromItsTextKey) {
  // Part 3 §8.5.2.3 extends "mul" with an "r" key; the text lives under the
  // same "t" key, and the substitutions are left unapplied.
  const LocalizedText qst{
      "qst", u"{\"t\":[[\"en\",\"I'm your text @1@\"]],\"r\":[[\"@1@\",1.2345]]}"};
  const std::vector<LocalizedText> decoded = DecodeMultiLanguage(qst);
  ASSERT_EQ(1u, decoded.size());
  EXPECT_EQ(u"I'm your text @1@", decoded.front().text);
}

TEST(LocaleNegotiationTest, ResolveSelectsThroughAPackedValue) {
  // The shape every caller with one stored attribute value has.
  const std::vector<LocalizedText> translations{Ru(), En()};
  const LocalizedText stored = EncodeMultiLanguage(translations);
  const std::vector<String> requested{"en"};
  EXPECT_EQ(En(), ResolveLocalizedText(stored, requested));
  EXPECT_EQ(Ru(), ResolveLocalizedText(stored, std::span<const String>{}));
  // A plain stored value is returned whatever the session asked for.
  EXPECT_EQ(Ru(), ResolveLocalizedText(Ru(), requested));
}

TEST(LocaleNegotiationTest, NonAsciiTextSurvivesThePackedForm) {
  const std::vector<LocalizedText> translations{
      LocalizedText{"ru", u"Отрадная 110 КВ"},
      LocalizedText{"en", u"Otradnaya 110 kV"}};
  EXPECT_EQ(translations,
            DecodeMultiLanguage(EncodeMultiLanguage(translations)));
}

TEST(LocaleNegotiationTest, IsSpecialLocaleRecognisesBothSpellings) {
  EXPECT_TRUE(IsSpecialLocale("mul"));
  EXPECT_TRUE(IsSpecialLocale("qst"));
  EXPECT_TRUE(IsSpecialLocale("MUL"));
  EXPECT_FALSE(IsSpecialLocale("en"));
  EXPECT_FALSE(IsSpecialLocale(""));
}

TEST(LocaleNegotiationTest, EmptyLocaleNeverMatches) {
  // A translation with no locale id cannot satisfy a request for a specific
  // one, but it remains available as the fallback.
  const std::vector<LocalizedText> translations{LocalizedText{u"Untagged"}};
  const std::vector<String> requested{"en"};
  EXPECT_FALSE(LocaleMatches("", "en"));
  EXPECT_FALSE(LocaleMatches("en", ""));
  EXPECT_EQ(LocalizedText{u"Untagged"},
            SelectLocalizedText(translations, requested));
}

// --- Composing a message that exists in several languages -----------------

TEST(LocaleNegotiationTest, AppendingToAPlainValueJustAppends) {
  // An untranslated message composes exactly as it did before any of this.
  EXPECT_EQ(LocalizedText(u"Value > 42"),
            AppendToEachLanguage(LocalizedText{u"Value >"}, u" 42"));
}

TEST(LocaleNegotiationTest, AppendingReachesEveryLanguage) {
  // The formatted limit is locale-neutral, but it has to land inside each
  // translation rather than on the packed payload.
  const std::vector<LocalizedText> translations{
      {"en", u"Alarm: Value >"}, {"ru", u"Тревога: Значение >"}};
  const LocalizedText appended =
      AppendToEachLanguage(EncodeMultiLanguage(translations), u" 42");

  EXPECT_EQ((std::vector<LocalizedText>{{"en", u"Alarm: Value > 42"},
                                        {"ru", u"Тревога: Значение > 42"}}),
            DecodeMultiLanguage(appended));
}

TEST(LocaleNegotiationTest, AppendingNothingChangesNothing) {
  const LocalizedText packed = EncodeMultiLanguage(
      std::vector<LocalizedText>{{"en", u"a"}, {"ru", u"б"}});
  EXPECT_EQ(packed, AppendToEachLanguage(packed, u""));
}

TEST(LocaleNegotiationTest, JoiningMatchesLanguageToLanguage) {
  // The point of the whole helper: one stored value that reads correctly in
  // either language, rather than two fragments glued in whichever language
  // the first one happened to be in.
  const LocalizedText first = EncodeMultiLanguage(
      std::vector<LocalizedText>{{"en", u"State change"},
                                 {"ru", u"Изменение состояния"}});
  const LocalizedText second = EncodeMultiLanguage(
      std::vector<LocalizedText>{{"en", u"Manual input"},
                                 {"ru", u"Ручной ввод"}});

  EXPECT_EQ((std::vector<LocalizedText>{
                {"en", u"State change; Manual input"},
                {"ru", u"Изменение состояния; Ручной ввод"}}),
            DecodeMultiLanguage(JoinLanguages(first, u"; ", second)));
}

TEST(LocaleNegotiationTest, JoiningAPlainFragmentUsesItForEveryLanguage) {
  // A fragment the catalog has not translated: better a mixed-language
  // message than a language that silently loses a sentence.
  const LocalizedText first = EncodeMultiLanguage(
      std::vector<LocalizedText>{{"en", u"State change"},
                                 {"ru", u"Изменение состояния"}});

  EXPECT_EQ((std::vector<LocalizedText>{{"en", u"State change; KP-02"},
                                        {"ru", u"Изменение состояния; KP-02"}}),
            DecodeMultiLanguage(
                JoinLanguages(first, u"; ", LocalizedText{u"KP-02"})));
}

TEST(LocaleNegotiationTest, JoiningIgnoresLanguagesOnlyTheFragmentHas) {
  // A de-only fragment must not create a German message holding nothing but
  // that fragment.
  const LocalizedText first = EncodeMultiLanguage(
      std::vector<LocalizedText>{{"en", u"State change"},
                                 {"ru", u"Изменение состояния"}});
  const LocalizedText second = EncodeMultiLanguage(
      std::vector<LocalizedText>{{"de", u"Handeingabe"},
                                 {"en", u"Manual input"}});

  const std::vector<LocalizedText> joined =
      DecodeMultiLanguage(JoinLanguages(first, u"; ", second));
  EXPECT_EQ(2u, joined.size());
  EXPECT_EQ(u"State change; Manual input", joined[0].text);
  // ru had no match, so it took the fragment's own authored language.
  EXPECT_EQ(String{"ru"}, joined[1].locale);
}

TEST(LocaleNegotiationTest, JoiningAnEmptySideYieldsTheOther) {
  // The first fragment of a message: there is nothing to join it to yet.
  const LocalizedText packed = EncodeMultiLanguage(
      std::vector<LocalizedText>{{"en", u"a"}, {"ru", u"б"}});
  EXPECT_EQ(packed, JoinLanguages(LocalizedText{}, u"; ", packed));
  EXPECT_EQ(packed, JoinLanguages(packed, u"; ", LocalizedText{}));
}

TEST(LocaleNegotiationTest, JoiningTwoPlainValuesStaysPlain) {
  // Nothing translated anywhere: the result must not become a packed value.
  const LocalizedText joined =
      JoinLanguages(LocalizedText{u"a"}, u"; ", LocalizedText{u"b"});
  EXPECT_EQ(LocalizedText(u"a; b"), joined);
  EXPECT_TRUE(joined.locale.empty());
}

}  // namespace
}  // namespace scada
