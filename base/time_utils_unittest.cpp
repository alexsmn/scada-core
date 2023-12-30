#include "base/time_utils.h"

#include "base/format_time.h"
#include "base/test/test_time.h"

#include <gmock/gmock.h>
#include <sstream>

using namespace testing;

TEST(TimeUtils, Test) {
  const char str[] = "Tue, 15 Nov 1994 12:45:26 GMT";
  auto base_time = TestTimeFromString(str);
  auto chrono_time = AsChrono(base_time);
  std::stringstream s;
  s << chrono_time;
  // TODO: Format chrono time.
  // EXPECT_EQ(s.str(), str);
}