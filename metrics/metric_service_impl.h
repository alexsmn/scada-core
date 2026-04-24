#pragma once

#include "base/any_executor.h"
#include "base/common_types.h"
#include "metrics/metric_service.h"

// Thread-safe.
class MetricServiceImpl final : public MetricService {
 public:
  MetricServiceImpl(AnyExecutor executor,
                    Duration report_metrics_period);

  // MetricService
  virtual void RegisterProvider(const Provider& provider) override;
  virtual void RegisterSink(const Sink& sink) override;

 private:
  class ProviderReporter;

  const AnyExecutor executor_;
  const Duration report_metrics_period_;

  std::vector<Sink> sinks_;
  Sink broadcast_sink_;
};
