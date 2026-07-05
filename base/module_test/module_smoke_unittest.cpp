// Smoke test for the scada.base module facade: this TU uses no base/ header
// includes — every base name comes from `import scada.base;` — to prove the
// exported surface is usable on its own.

// A TU that *defines* coroutines needs std::coroutine_traits textually even
// when every coroutine-related name it uses comes from the import.
#include <coroutine>
#include <filesystem>
#include <optional>
#include <string>

#include <boost/asio/io_context.hpp>
#include <gtest/gtest.h>

// Import after the textual includes: the reverse order trips an AppleClang 21
// declaration-merging bug in libc++ (dependent alias template __promote_t:
// "type alias template redefinition with different types").
import scada.base;

namespace scada_base_module {
namespace {

TEST(ScadaBaseModuleSmoke, CheckAndNotReached) {
  base::Check(true, "module smoke");
  base::Check(std::make_optional(1), [] { return std::string("lazy"); });
  EXPECT_TRUE(true);
}

TEST(ScadaBaseModuleSmoke, FormatAndParse) {
  EXPECT_EQ(Format(42), "42");
  EXPECT_EQ(Format(3.5), "3.5");
  int value = 0;
  EXPECT_TRUE(Parse("123", value));
  EXPECT_EQ(value, 123);
  EXPECT_EQ(ParseWithDefault<int>("bogus", 7), 7);
}

struct Observer {
  int notified = 0;
};

TEST(ScadaBaseModuleSmoke, ObserverList) {
  base::ObserverList<Observer> observers;
  Observer observer;
  observers.AddObserver(&observer);
  EXPECT_TRUE(observers.HasObserver(&observer));
  observers.RemoveObserver(&observer);
  EXPECT_FALSE(observers.HasObserver(&observer));
}

TEST(ScadaBaseModuleSmoke, StringUtil) {
  EXPECT_TRUE(IEqualsAscii("SCADA", "scada"));
  EXPECT_EQ(SplitString("a,b", ',').size(), 2u);
  EXPECT_EQ(UtfConvert<char>(u"abc"), "abc");
}

TEST(ScadaBaseModuleSmoke, Tier2TimeAndUtilities) {
  base::TimeDelta delta = base::TimeDelta::FromSeconds(90);
  EXPECT_EQ(delta.InMinutes(), 1);
  base::Time now = base::Time::Now();
  EXPECT_FALSE(FormatTime(now).empty());

  EXPECT_EQ(base::MD5String("abc"), "900150983cd24fb0d6963f7d28e17f72");

  std::string encoded;
  base::Base64Encode("scada", &encoded);
  std::string decoded;
  EXPECT_TRUE(base::Base64Decode(encoded, &decoded));
  EXPECT_EQ(decoded, "scada");

  std::filesystem::path exe_dir;
  EXPECT_TRUE(base::PathService::Get(base::DIR_EXE, &exe_dir));
  EXPECT_FALSE(exe_dir.empty());
}

TEST(ScadaBaseModuleSmoke, Tier3BoostFacingNames) {
  Interval<int> a{1, 5};
  Interval<int> b{2, 3};
  EXPECT_TRUE(IntervalContains(a, b));
  EXPECT_TRUE(IntervalsOverlap(a, b));

  EXPECT_EQ(u16format(L"{}-{}", 1, u"x"), u"1-x");

  RateLimiter limiter{1};
  EXPECT_TRUE(limiter.Request());

  // Coroutine surface: spawn a trivial coroutine on an asio executor
  // obtained through the exported AnyExecutor alias.
  boost::asio::io_context context;
  bool ran = false;
  CoSpawn(context.get_executor(), [&]() -> Awaitable<void> {
    ran = true;
    co_return;
  });
  context.run();
  EXPECT_TRUE(ran);
}

}  // namespace
}  // namespace scada_base_module
