#include "base/ui_text.h"

#include "scada/qualifier.h"
#include "scada/status.h"
#include "scada/variant.h"

#include <gtest/gtest.h>

namespace scada {
namespace {

// Answers with a recognizable stand-in, so the assertions do not depend on a
// real catalog and say plainly *which* English source was looked up.
std::u16string RecordingTranslator(std::string_view english) {
  return u"<" + std::u16string{english.begin(), english.end()} + u">";
}

class CoreUiTextTest : public ::testing::Test {
 protected:
  void TearDown() override { SetUiTextTranslator(nullptr); }
};

// The regression these three guard: every one of these strings used to be a
// Russian literal compiled into `core`, which no catalog could reach. A client
// running in any other language rendered them in Russian permanently, and
// nothing reported it. They must now go through the seam.

TEST_F(CoreUiTextTest, StatusDescriptionsGoThroughTheTranslator) {
  SetUiTextTranslator(&RecordingTranslator);

  EXPECT_EQ(ToString16(StatusCode::Good),
            u"<Operation completed successfully>");
  EXPECT_EQ(ToString16(StatusCode::Bad), u"<Error>");
  EXPECT_EQ(ToString16(StatusCode::Bad_Timeout),
            u"<Operation aborted after the wait timed out>");
  EXPECT_EQ(ToString16(Status{StatusCode::Bad_UserAccessDenied}),
            u"<Not enough rights to perform the operation>");
}

// The unmapped-code fallbacks are the easiest half to forget: they are a second
// pair of literals, reached only by a code absent from the table.
TEST_F(CoreUiTextTest, StatusFallbacksGoThroughTheTranslator) {
  SetUiTextTranslator(&RecordingTranslator);

  const auto unknown_good = static_cast<StatusCode>(0x0FFF);
  const auto unknown_bad = static_cast<StatusCode>(
      (static_cast<unsigned>(StatusSeverity::Bad) << 14) | 0x3FFF);

  EXPECT_EQ(ToString16(unknown_good), u"<Operation completed successfully>");
  EXPECT_EQ(ToString16(unknown_bad), u"<Error>");
}

TEST_F(CoreUiTextTest, QualifierFlagsGoThroughTheTranslator) {
  SetUiTextTranslator(&RecordingTranslator);

  EXPECT_EQ(ToString16(Qualifier{Qualifier::MANUAL}), u"<Manual> ");
  EXPECT_EQ(ToString16(Qualifier{Qualifier::BAD | Qualifier::OFFLINE}),
            u"<Bad quality> <No link> ");
}

TEST_F(CoreUiTextTest, BooleanLabelsGoThroughTheTranslator) {
  SetUiTextTranslator(&RecordingTranslator);

  EXPECT_EQ(Variant::TrueLabel(), u"<Yes>");
  EXPECT_EQ(Variant::FalseLabel(), u"<No>");
}

// Without a catalog — the server, and every unit test that does not install
// one — the English source is what renders. Not a placeholder: the tiers ship
// no `.qm` files at all, so this is their permanent behaviour.
TEST_F(CoreUiTextTest, WithoutATranslatorTheEnglishSourceRenders) {
  SetUiTextTranslator(nullptr);

  EXPECT_EQ(ToString16(StatusCode::Bad_Timeout),
            u"Operation aborted after the wait timed out");
  EXPECT_EQ(ToString16(Qualifier{Qualifier::STALE}), u"Stale ");
  EXPECT_EQ(Variant::TrueLabel(), u"Yes");
  EXPECT_EQ(Variant::FalseLabel(), u"No");
}

// The compact letter form and the spelled-out form are driven by one table, so
// they cannot disagree about which flags are set.
TEST_F(CoreUiTextTest, LetterFormTracksTheSameFlags) {
  const Qualifier qualifier{Qualifier::BAD | Qualifier::MANUAL |
                            Qualifier::STALE};

  EXPECT_EQ(ToString(qualifier), "BMT");
  EXPECT_EQ(ToString16(qualifier), u"Bad quality Manual Stale ");
}

}  // namespace
}  // namespace scada
