#pragma once

#include "base/boost_log.h"
#include "metrics/metrics.h"

class BoostLogMetricReporter {
 public:
  void Report(const Metrics& metrics) {
    metrics.Visit(
        [&logger = logger_](const std::string& name, const MetricValue& value) {
          LOG_INFO(logger) << "Metric" << LOG_TAG("Name", name)
                           << LOG_TAG("Value", value);
        });
  }

 private:
  BoostLogger logger_{LOG_NAME("Metrics")};
};
