#pragma once

#include "base/common_types.h"
#include "metrics/metric_service.h"

class Executor;

class MetricServiceImpl final : public MetricService {
 public:
  using MetricReporter = std::function<void(const Metrics& metrics)>;

  MetricServiceImpl(std::shared_ptr<Executor> executor,
                    Duration report_metrics_period,
                    const MetricReporter& metric_reporter);

  // MetricService
  virtual void RegisterProvider(const Provider& provider) override;

 private:
  class ProviderReporter;

  const std::shared_ptr<Executor> executor_;
  const Duration report_metrics_period_;
  const MetricReporter metric_reporter_;
};