#include "scada/data_value.h"

#include <gtest/gtest.h>

#include <string>
#include <vector>

namespace scada {
namespace {

// A value-less DataValue must never leave the server at Good severity: an empty
// Variant carries no contents and no datatype, so "Good" would tell the client
// a non-existent result is usable. OPC UA Part 4 §7.38.2 names the code for
// this state, and Part 8 §7.3 pairs a null value with Bad severity.
TEST(ReportedStatusCodeTest, EmptyVariantAtGoodBecomesWaitingForInitialData) {
  DataValue data_value;
  ASSERT_TRUE(data_value.value.is_null());
  ASSERT_EQ(data_value.status_code, StatusCode::Good);

  EXPECT_EQ(ReportedStatusCode(data_value),
            StatusCode::Bad_WaitingForInitialData);
}

// The observed GCP-demo shape: timestamps set, quality bits already saying the
// device is offline, but no value ever delivered. `DataValue::is_null()` is
// false here (it also requires a zero Qualifier), which is exactly why the
// rule tests the Variant rather than the whole DataValue.
TEST(ReportedStatusCodeTest, EmptyVariantWithQualityBitsStillNotGood) {
  DataValue data_value;
  data_value.qualifier.set_online(false);
  data_value.source_timestamp = Now();
  data_value.server_timestamp = data_value.source_timestamp;

  ASSERT_FALSE(data_value.is_null());
  ASSERT_TRUE(data_value.value.is_null());

  EXPECT_EQ(ReportedStatusCode(data_value),
            StatusCode::Bad_WaitingForInitialData);
}

// A Good subcode is still a Good severity, so it cannot stand over nothing
// either.
TEST(ReportedStatusCodeTest, GoodSubcodeOverEmptyVariantIsReplaced) {
  DataValue data_value;
  data_value.status_code = StatusCode::Good_Manual;

  EXPECT_EQ(ReportedStatusCode(data_value),
            StatusCode::Bad_WaitingForInitialData);
}

// A producer that already reported a specific reason keeps it — the rule only
// fills the gap where nothing was reported at all.
TEST(ReportedStatusCodeTest, ExistingNonGoodStatusIsPreserved) {
  DataValue data_value;
  data_value.status_code = StatusCode::Bad_WrongNodeId;
  EXPECT_EQ(ReportedStatusCode(data_value), StatusCode::Bad_WrongNodeId);

  DataValue uncertain;
  uncertain.status_code = StatusCode::Uncertain_Disconnected;
  EXPECT_EQ(ReportedStatusCode(uncertain), StatusCode::Uncertain_Disconnected);
}

// A value that is actually present with clean quality reports Good, including
// the falsy-looking ones.
TEST(ReportedStatusCodeTest, PresentValueKeepsItsStatus) {
  const Time now = Now();

  EXPECT_EQ(ReportedStatusCode(DataValue{Variant{0}, {}, now, now}),
            StatusCode::Good);
  EXPECT_EQ(ReportedStatusCode(DataValue{Variant{false}, {}, now, now}),
            StatusCode::Good);
  EXPECT_EQ(ReportedStatusCode(DataValue{Variant{String{}}, {}, now, now}),
            StatusCode::Good);
  // An empty array is a typed value, not an absent one.
  EXPECT_EQ(ReportedStatusCode(
                DataValue{Variant{std::vector<Int32>{}}, {}, now, now}),
            StatusCode::Good);
}

// A delivered value whose quality bits say it is not trustworthy must not be
// published at Good either — the Qualifier is a SCADA extension field a
// standard OPC UA client never decodes, so the quality has to reach it as a
// StatusCode.
TEST(ReportedStatusCodeTest, PresentValueProjectsItsQualifier) {
  const Time now = Now();
  const auto with_quality = [now](unsigned flags) {
    return ReportedStatusCode(
        DataValue{Variant{42}, Qualifier{flags}, now, now});
  };

  EXPECT_EQ(with_quality(Qualifier::OFFLINE),
            StatusCode::Uncertain_Disconnected);
  EXPECT_EQ(with_quality(Qualifier::STALE), StatusCode::Uncertain_NotUpdated);
  EXPECT_EQ(with_quality(Qualifier::MISCONFIGURED),
            StatusCode::Uncertain_Misconfigured);
  EXPECT_EQ(with_quality(Qualifier::BAD), StatusCode::Uncertain_DeviceFlag);
  EXPECT_EQ(with_quality(Qualifier::MANUAL), StatusCode::Good_Manual);
  EXPECT_EQ(with_quality(Qualifier::SIMULATED), StatusCode::Good_Simulated);
}

// The value constructor already stamps Bad for a FAILED qualifier, so the
// terminal case is caught by the non-Good rule before the projection runs; the
// projection agrees with it either way.
TEST(ReportedStatusCodeTest, FailedQualifierReportsBad) {
  const Time now = Now();
  const DataValue failed{Variant{42}, Qualifier{Qualifier::FAILED}, now, now};
  ASSERT_EQ(failed.status_code, StatusCode::Bad);
  EXPECT_EQ(ReportedStatusCode(failed), StatusCode::Bad);

  // Same answer when the qualifier was mutated after construction, which is
  // what `VariableHandle::UpdateQualifier` does and which leaves `status_code`
  // untouched.
  DataValue mutated{Variant{42}, {}, now, now};
  mutated.qualifier.set_failed(true);
  ASSERT_EQ(mutated.status_code, StatusCode::Good);
  EXPECT_EQ(ReportedStatusCode(mutated), StatusCode::Bad);
}

// An explicit non-Good status is the producer's specific reason and outranks
// the quality projection, which could only soften it.
TEST(ReportedStatusCodeTest, ExplicitStatusOutranksQualifier) {
  const Time now = Now();
  DataValue data_value{Variant{42}, Qualifier{Qualifier::MANUAL}, now, now};
  data_value.status_code = StatusCode::Bad_UserAccessDenied;
  EXPECT_EQ(ReportedStatusCode(data_value), StatusCode::Bad_UserAccessDenied);
}

}  // namespace
}  // namespace scada
