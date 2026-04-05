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
                   const Sink& sink)
      : executor_{std::move(executor)},
        report_metrics_period_{report_metrics_period},
        provider_{provider},
        sink_{sink} {}

  void Schedule() {
    executor_->PostDelayedTask(
        report_metrics_period_,
        std::bind_front(&ProviderReporter::Report, shared_from_this()));
  }

 private:
  void Report() {
    // Cycling stops when the promise is rejected.
    provider_().then(BindExecutor(
        executor_, [this, ref = shared_from_this()](const Metrics& metrics) {
          sink_(metrics);
          Schedule();
        }));
  }

  const std::shared_ptr<Executor> executor_;
  const Duration report_metrics_period_;
  const Provider provider_;
  const Sink sink_;
};

// MetricServiceImpl

MetricServiceImpl::MetricServiceImpl(std::shared_ptr<Executor> executor,
                                     Duration report_metrics_period)
    : executor_{std::move(executor)},
      report_metrics_period_{report_metrics_period} {
  broadcast_sink_ = [this](const Metrics& metrics) {
    for (const auto& sink : sinks_) {
      sink(metrics);
    }
  };
}

void MetricServiceImpl::RegisterProvider(const Provider& provider) {
  std::make_shared<ProviderReporter>(executor_, report_metrics_period_,
                                     provider, broadcast_sink_)
      ->Schedule();
}

void MetricServiceImpl::RegisterSink(const Sink& sink) {
  Dispatch(*executor_, [this, sink] { sinks_.emplace_back(sink); });
}
