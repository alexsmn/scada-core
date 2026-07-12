#include "base/time/time_wire_codec.h"

#include "base/time/time.h"

#include <gtest/gtest.h>

namespace scada::base {
namespace {

// Golden characterization of the on-wire / on-disk time encodings. These pin the
// EXACT values that gRPC messages, SQLite history rows, the sync resume cursor,
// and saved graph-view files depend on, so any future change to Time's in-memory
// representation that would alter a persisted/transmitted timestamp fails here.

// Microseconds from the Windows 1601 epoch to the Unix 1970 epoch.
constexpr int64_t kUnixEpochWire = 11644473600000000LL;

TEST(TimeWireCodecTest, WireMicrosecondsRoundTripsAndPinsUnixEpoch) {
  // The µs-since-1601 wire value round-trips exactly.
  const int64_t wire = kUnixEpochWire + 1'500'000;  // 1.5s after the Unix epoch
  const Time t = DecodeWireTime(wire);
  EXPECT_EQ(EncodeWireMicroseconds(t), wire);

  // The Unix epoch pins the offset constant that both epochs share.
  EXPECT_EQ(EncodeWireMicroseconds(Time::UnixEpoch()), kUnixEpochWire);

  // A null time is wire value 0.
  EXPECT_EQ(EncodeWireMicroseconds(Time()), 0);
  EXPECT_TRUE(DecodeWireTime(0).is_null());
}

TEST(TimeWireCodecTest, WireDurationRoundTrips) {
  EXPECT_EQ(EncodeWireMicroseconds(TimeDelta::FromSeconds(5)), 5'000'000);
  EXPECT_EQ(DecodeWireDelta(5'000'000), TimeDelta::FromSeconds(5));

  // Sentinels survive the round-trip (used as "unbounded" range/timeout).
  EXPECT_TRUE(DecodeWireDelta(EncodeWireMicroseconds(TimeDelta::Max())).is_max());
  EXPECT_TRUE(DecodeWireDelta(EncodeWireMicroseconds(TimeDelta::Min())).is_min());
}

TEST(TimeWireCodecTest, DoubleTUsesUnixEpochAndPreservesNullQuirk) {
  // DoubleT is seconds since the Unix epoch (distinct from the µs-1601 wire).
  EXPECT_EQ(EncodeDoubleT(Time::UnixEpoch()), 0.0);

  // Historical quirk: a null Time encodes to 0.0, and 0.0 decodes back to a
  // NULL time (not the Unix epoch). This asymmetry is deliberate and must hold.
  EXPECT_EQ(EncodeDoubleT(Time()), 0.0);
  EXPECT_TRUE(DecodeDoubleT(0.0).is_null());

  // A non-zero value round-trips (picked to be exactly representable).
  const Time t = DecodeWireTime(kUnixEpochWire + 1'500'000);
  EXPECT_EQ(EncodeDoubleT(t), 1.5);
  EXPECT_EQ(DecodeDoubleT(1.5), t);
}

}  // namespace
}  // namespace scada::base
