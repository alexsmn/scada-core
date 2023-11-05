#pragma once

#include "base/common_types.h"
#include "metrics/metric_service.h"

class Executor;

class MetricServiceImpl final : public MetricService {
 public:
  MetricServiceImpl(std::shared_ptr<Executor> executor,
                    Duration report_metrics_period);

  // MetricService
  virtual void RegisterProvider(const Provider& provider) override;
  virtual void RegisterSink(const Sink& sink) override;

 private:
  class ProviderReporter;

  const std::shared_ptr<Executor> executor_;
  const Duration report_metrics_period_;

  std::vector<Sink> sinks_;
  Sink fork_sink_;
};