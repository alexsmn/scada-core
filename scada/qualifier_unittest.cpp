#include "scada/qualifier.h"

#include <gtest/gtest.h>

namespace scada {
namespace {

TEST(QualifierToStatusTest, ClearQualifierIsGood) {
  EXPECT_EQ(Qualifier{}.ToStatus().code(), StatusCode::Good);
}

// FAILED is terminal, so it outranks every other bit and can never come out at
// Good or Uncertain severity. `DataValue`'s value constructor applies the same
// rule, and `SubscriptionStub::OnDataChange` asserts it as an invariant.
TEST(QualifierToStatusTest, FailedOutranksEveryOtherFlag) {
  EXPECT_EQ(Qualifier{Qualifier::FAILED}.ToStatus().code(), StatusCode::Bad);

  // Including the flags that would otherwise win, and the ones that would
  // otherwise project onto a Good subcode.
  EXPECT_EQ(Qualifier{Qualifier::FAILED | Qualifier::BAD}.ToStatus().code(),
            StatusCode::Bad);
  EXPECT_EQ(Qualifier{Qualifier::FAILED | Qualifier::OFFLINE}.ToStatus().code(),
            StatusCode::Bad);
  EXPECT_EQ(Qualifier{Qualifier::FAILED | Qualifier::MANUAL}.ToStatus().code(),
            StatusCode::Bad);
}

TEST(QualifierToStatusTest, BadQualityFlagsProjectOntoUncertain) {
  EXPECT_EQ(Qualifier{Qualifier::BAD}.ToStatus().code(),
            StatusCode::Uncertain_DeviceFlag);
  EXPECT_EQ(Qualifier{Qualifier::OFFLINE}.ToStatus().code(),
            StatusCode::Uncertain_Disconnected);
  EXPECT_EQ(Qualifier{Qualifier::STALE}.ToStatus().code(),
            StatusCode::Uncertain_NotUpdated);
  EXPECT_EQ(Qualifier{Qualifier::MISCONFIGURED}.ToStatus().code(),
            StatusCode::Uncertain_Misconfigured);
}

TEST(QualifierToStatusTest, UsableFlagsProjectOntoGoodSubcodes) {
  EXPECT_EQ(Qualifier{Qualifier::MANUAL}.ToStatus().code(),
            StatusCode::Good_Manual);
  EXPECT_EQ(Qualifier{Qualifier::BACKUP}.ToStatus().code(),
            StatusCode::Good_Backup);
  EXPECT_EQ(Qualifier{Qualifier::SIMULATED}.ToStatus().code(),
            StatusCode::Good_Simulated);
  EXPECT_EQ(Qualifier{Qualifier::SPORADIC}.ToStatus().code(),
            StatusCode::Good_Sporadic);
}

// A bad-quality flag wins over a usable one: an offline reading that was also
// entered manually is still not something a client should act on.
TEST(QualifierToStatusTest, BadQualityWinsOverUsableFlags) {
  EXPECT_EQ(Qualifier{Qualifier::OFFLINE | Qualifier::MANUAL}.ToStatus().code(),
            StatusCode::Uncertain_Disconnected);
  EXPECT_EQ(Qualifier{Qualifier::STALE | Qualifier::SPORADIC}.ToStatus().code(),
            StatusCode::Uncertain_NotUpdated);
}

// The limit bits ride alongside the code. They are dropped when a DataValue
// reports the projection (a DataValue stores only a StatusCode), but the
// projection itself carries them for callers that can.
TEST(QualifierToStatusTest, LimitBitsAreCarried) {
  Qualifier low;
  low.set_limit(Qualifier::LIMIT_LOLO);
  EXPECT_EQ(low.ToStatus().limit(), StatusLimit::Low);

  Qualifier high;
  high.set_limit(Qualifier::LIMIT_HIHI);
  EXPECT_EQ(high.ToStatus().limit(), StatusLimit::High);

  // The soft limits carry no StatusCode limit bits.
  Qualifier soft_high;
  soft_high.set_limit(Qualifier::LIMIT_HI);
  EXPECT_EQ(soft_high.ToStatus().limit(), StatusLimit::None);
}

}  // namespace
}  // namespace scada
