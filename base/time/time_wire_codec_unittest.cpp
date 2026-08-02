#include "base/time/time_wire_codec.h"

#include "base/time/time.h"

#include <gtest/gtest.h>

#include <chrono>
#include <limits>

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

  // The Unix epoch (default-constructed Time) pins the shared offset constant.
  EXPECT_EQ(EncodeWireMicroseconds(Time{}), kUnixEpochWire);

  // A null time is wire value 0.
  EXPECT_EQ(EncodeWireMicroseconds(kNullTime), 0);
  EXPECT_TRUE(IsNull(DecodeWireTime(0)));
}

TEST(TimeWireCodecTest, WireDurationRoundTrips) {
  EXPECT_EQ(EncodeWireMicroseconds(std::chrono::seconds{5}), 5'000'000);
  EXPECT_EQ(DecodeWireDelta(5'000'000), std::chrono::seconds{5});

  // Sentinels survive the round-trip (used as "unbounded" range/timeout).
  EXPECT_EQ(DecodeWireDelta(EncodeWireMicroseconds(Duration::max())),
            Duration::max());
  EXPECT_EQ(DecodeWireDelta(EncodeWireMicroseconds(Duration::min())),
            Duration::min());
}

TEST(TimeWireCodecTest, WireMicrosecondsPinsGoldenValues) {
  // Exact int64 µs-since-1601 wire values that persisted data depends on. These
  // must survive the migration of Time's in-memory representation unchanged.
  struct GoldenCase {
    Time time;
    int64_t wire;
  };
  const GoldenCase cases[] = {
      // A null time is wire value 0 (0 ticks since the 1601 epoch).
      {kNullTime, 0},
      // The Unix epoch.
      {Time{}, kUnixEpochWire},
      // A pre-1970 instant: one second before the Unix epoch. Exercises the
      // negative-relative-to-Unix but positive-relative-to-1601 range.
      {DecodeWireTime(kUnixEpochWire - 1'000'000), kUnixEpochWire - 1'000'000},
      // The unbounded-range sentinels.
      {kMaxTime, std::numeric_limits<int64_t>::max()},
      {kMinTime, std::numeric_limits<int64_t>::min()},
  };
  for (const auto& c : cases) {
    EXPECT_EQ(EncodeWireMicroseconds(c.time), c.wire);
    EXPECT_EQ(DecodeWireTime(c.wire), c.time);
  }
}

TEST(TimeWireCodecTest, DoubleTUsesUnixEpochAndPreservesNullQuirk) {
  // DoubleT is seconds since the Unix epoch (distinct from the µs-1601 wire).
  EXPECT_EQ(EncodeDoubleT(Time{}), 0.0);

  // Historical quirk: a null Time encodes to 0.0, and 0.0 decodes back to a
  // NULL time (not the Unix epoch). This asymmetry is deliberate and must hold.
  EXPECT_EQ(EncodeDoubleT(kNullTime), 0.0);
  EXPECT_TRUE(IsNull(DecodeDoubleT(0.0)));

  // A non-zero value round-trips (picked to be exactly representable).
  const Time t = DecodeWireTime(kUnixEpochWire + 1'500'000);
  EXPECT_EQ(EncodeDoubleT(t), 1.5);
  EXPECT_EQ(DecodeDoubleT(1.5), t);
}

}  // namespace
}  // namespace scada::base
