#include "metrics/metrics.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using namespace testing;

TEST(MetricsTest, EmptyIncludesAttributes) {
  Metrics metrics;
  EXPECT_TRUE(metrics.empty());

  metrics.SetAttribute("database_node_id", "ns=100;i=1");

  EXPECT_FALSE(metrics.empty());
}

TEST(MetricsTest, StoresAttributes) {
  Metrics metrics;

  metrics.SetAttribute("database_node_id", "ns=100;i=1");
  metrics.SetAttribute("database_node_id", "ns=100;i=2");
  metrics.SetAttribute("server", "main");

  EXPECT_THAT(metrics.attributes(),
              UnorderedElementsAre(Pair("database_node_id", "ns=100;i=2"),
                                   Pair("server", "main")));
}
