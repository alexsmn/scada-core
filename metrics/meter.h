#pragma once

#include <cstdint>
#include <map>
#include <memory>
#include <string>
#include <string_view>

namespace scada::metrics {

using MetricAttributes = std::map<std::string, std::string>;

// Records measurements through an OpenTelemetry meter and caches instruments.
class Meter {
 public:
  explicit Meter(std::string meter_name);
  Meter(std::string meter_name, MetricAttributes attributes);
  ~Meter();

  Meter(const Meter&) = delete;
  Meter& operator=(const Meter&) = delete;

  void AddCounter(std::string_view metric_name,
                  std::uint64_t value,
                  const MetricAttributes& attributes = {});
  void AddUpDownCounter(std::string_view metric_name,
                        std::int64_t delta,
                        const MetricAttributes& attributes = {});
  void RecordHistogram(std::string_view metric_name,
                       double value,
                       const MetricAttributes& attributes = {});

 private:
  class Impl;
  std::unique_ptr<Impl> impl_;
};

}  // namespace scada::metrics
