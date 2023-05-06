#include "dump.h"

#include <gmock/gmock.h>

using namespace testing;

TEST(DumpTest, GetDumpFileName) {
  EXPECT_THAT(GetDumpFileName("prefix"), StartsWith("prefix"));
}
