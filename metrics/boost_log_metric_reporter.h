#pragma once

#include "base/boost_log.h"
#include "metrics/metrics.h"

class BoostLogMetricReporter {
 public:
  void Report(const Metrics& metrics) {
    using E = std::pair<std::string /*name*/, MetricValue>;
    std::vector<E> report;
    std::ranges::sort(report, {}, &E::first);

    metrics.Visit([&report](const std::string& name, const MetricValue& value) {
      report.emplace_back(name, value);
    });

    for (const auto& [name, value] : report) {
      std::visit(
          [&](const auto& raw_value) {
            LOG_INFO(logger_) << "Metric" << LOG_TAG("Name", name)
                              << LOG_TAG("Value", raw_value);
          },
          value);
    }
  }

 private:
  BoostLogger logger_{LOG_NAME("Metrics")};
};
