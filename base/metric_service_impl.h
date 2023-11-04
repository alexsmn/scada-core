#pragma once

#include "base/executor_timer.h"
#include "base/metric_service.h"

#include <algorithm>
#include <vector>

class MetricServiceImpl : public MetricService {
 public:
  using MetricReporter = std::function<void(const Metrics& metrics)>;

  MetricServiceImpl(std::shared_ptr<Executor> executor,
                    Duration report_metrics_period,
                    const MetricReporter& metric_reporter)
      : metric_reporter_{metric_reporter},
        report_metrics_timer_{std::move(executor)} {
    report_metrics_timer_.StartRepeating(
        report_metrics_period,
        std::bind_front(&MetricServiceImpl::ReportMetrics, this));
  }

  virtual void RegisterProvider(const Provider& provider,
                                const CancelationRef& cancelation) override {
    providers_.emplace_back(cancelation, provider);
  }

 private:
  // Collects metrics from providers that have not been canceled. Removes
  // canceled providers from the registered provider list.
  Metrics CollectMetrics() {
    auto canceled_providers = std::ranges::partition(
        providers_, [](const auto& p) { return !p.first.canceled(); });
    providers_.erase(canceled_providers.begin(), canceled_providers.end());

    Metrics metrics;
    for (const auto& [_, provider] : providers_) {
      provider(metrics);
    }
    return metrics;
  }

  void ReportMetrics() {
    auto metrics = CollectMetrics();
    metric_reporter_(metrics);
  }

  const MetricReporter metric_reporter_;

  std::vector<std::pair<CancelationRef, Provider>> providers_;

  ExecutorTimer report_metrics_timer_;
};