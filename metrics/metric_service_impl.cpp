#include "metrics/metric_service_impl.h"

#include "base/executor.h"
#include "metrics/metrics.h"

// MetricServiceImpl::ProviderReporter

class MetricServiceImpl::ProviderReporter
    : public std::enable_shared_from_this<ProviderReporter> {
 public:
  ProviderReporter(std::shared_ptr<Executor> executor,
                   Duration report_metrics_period,
                   const Provider& provider,
                   const MetricReporter& metric_reporter)
      : executor_{std::move(executor)},
        report_metrics_period_{report_metrics_period},
        provider_{provider},
        metric_reporter_{metric_reporter} {}

  void Schedule() {
    executor_->PostDelayedTask(
        report_metrics_period_,
        std::bind_front(&ProviderReporter::Report, shared_from_this()));
  }

 private:
  void Report() {
    provider_().then([this, ref = shared_from_this()](const Metrics& metrics) {
      metric_reporter_(metrics);
      Schedule();
    });
  }

  const std::shared_ptr<Executor> executor_;
  const Duration report_metrics_period_;
  const Provider provider_;
  const MetricReporter metric_reporter_;
};

// MetricServiceImpl

MetricServiceImpl::MetricServiceImpl(std::shared_ptr<Executor> executor,
                                     Duration report_metrics_period,
                                     const MetricReporter& metric_reporter)
    : executor_{std::move(executor)},
      report_metrics_period_{report_metrics_period},
      metric_reporter_{metric_reporter} {}

void MetricServiceImpl::RegisterProvider(const Provider& provider) {
  std::make_shared<ProviderReporter>(executor_, report_metrics_period_,
                                     provider, metric_reporter_)
      ->Schedule();
}
