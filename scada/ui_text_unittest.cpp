#include "scada/qualifier.h"
#include "scada/variant.h"

#include <gtest/gtest.h>

namespace scada {
namespace {

// Core carries no operator-facing words for quality flags or booleans: the
// Qt client installs them through these providers. The regression the tests
// guard is older than the providers — these strings were once Russian literals
// compiled into `core`, then English ones, and either way a client running in
// another language could not reach them.

std::u16string RecordingBooleanText(bool value) {
  return value ? u"<yes>" : u"<no>";
}

class CoreUiTextTest : public ::testing::Test {
 protected:
  void TearDown() override {
    SetQualifierFlagTextProvider(nullptr);
    SetBooleanTextProvider(nullptr);
  }
};

TEST_F(CoreUiTextTest, QualifierFlagsGoThroughTheProvider) {
  SetQualifierFlagTextProvider([](unsigned flag) -> std::u16string {
    return flag == Qualifier::MANUAL    ? u"<manual>"
           : flag == Qualifier::BAD     ? u"<bad>"
           : flag == Qualifier::OFFLINE ? u"<offline>"
                                        : u"<other>";
  });

  EXPECT_EQ(ToString16(Qualifier{Qualifier::MANUAL}), u"<manual> ");
  EXPECT_EQ(ToString16(Qualifier{Qualifier::BAD | Qualifier::OFFLINE}),
            u"<bad> <offline> ");
}

TEST_F(CoreUiTextTest, BooleanLabelsGoThroughTheProvider) {
  SetBooleanTextProvider(&RecordingBooleanText);

  EXPECT_EQ(Variant::TrueLabel(), u"<yes>");
  EXPECT_EQ(Variant::FalseLabel(), u"<no>");
}

// Without a provider — the server, and every unit test that does not install
// one — the invariant forms render: flag enum names and `true`/`false`.
TEST_F(CoreUiTextTest, WithoutAProviderTheInvariantFormsRender) {
  EXPECT_EQ(ToString16(Qualifier{Qualifier::STALE}), u"STALE ");
  EXPECT_EQ(ToString16(Qualifier{Qualifier::BAD | Qualifier::FAILED}),
            u"BAD FAILED ");
  EXPECT_EQ(Variant::TrueLabel(), u"true");
  EXPECT_EQ(Variant::FalseLabel(), u"false");
}

// The compact letter form and the spelled-out form are driven by one table, so
// they cannot disagree about which flags are set.
TEST_F(CoreUiTextTest, LetterFormTracksTheSameFlags) {
  const Qualifier qualifier{Qualifier::BAD | Qualifier::MANUAL |
                            Qualifier::STALE};

  EXPECT_EQ(ToString(qualifier), "BMT");
  EXPECT_EQ(ToString16(qualifier), u"BAD MANUAL STALE ");
}

}  // namespace
}  // namespace scada
