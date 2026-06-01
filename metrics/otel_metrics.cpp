#include "metrics/otel_metrics.h"

#include <opentelemetry/common/key_value_iterable_view.h>
#include <opentelemetry/exporters/otlp/otlp_grpc_metric_exporter_factory.h>
#include <opentelemetry/exporters/otlp/otlp_grpc_metric_exporter_options.h>
#include <opentelemetry/metrics/provider.h>
#include <opentelemetry/sdk/metrics/export/periodic_exporting_metric_reader_factory.h>
#include <opentelemetry/sdk/metrics/meter_context_factory.h>
#include <opentelemetry/sdk/metrics/meter_provider_factory.h>
#include <opentelemetry/sdk/metrics/view/view_registry_factory.h>
#include <opentelemetry/sdk/resource/resource.h>

#include <mutex>
#include <unordered_map>
#include <vector>

namespace metrics {

namespace {

namespace metrics_api = opentelemetry::metrics;
namespace metrics_sdk = opentelemetry::sdk::metrics;
namespace otlp = opentelemetry::exporter::otlp;
namespace resource = opentelemetry::sdk::resource;

std::string NormalizeGrpcEndpoint(std::string endpoint) {
  constexpr std::string_view kHttpPrefix = "http://";
  constexpr std::string_view kHttpsPrefix = "https://";

  if (endpoint.starts_with(kHttpPrefix)) {
    endpoint.erase(0, kHttpPrefix.size());
  } else if (endpoint.starts_with(kHttpsPrefix)) {
    endpoint.erase(0, kHttpsPrefix.size());
  }

  return endpoint;
}

template <class Instrument>
Instrument& FindOrCreateInstrument(
    std::unordered_map<std::string, std::unique_ptr<Instrument>>& instruments,
    std::mutex& mutex,
    std::string_view metric_name,
    auto create) {
  std::lock_guard lock{mutex};

  const std::string name{metric_name};
  auto [iter, inserted] = instruments.try_emplace(name);
  if (inserted) {
    iter->second = create(name);
  }

  return *iter->second;
}

std::vector<std::pair<opentelemetry::nostd::string_view,
                      opentelemetry::common::AttributeValue>>
MakeOtelAttributes(const MetricAttributes& attributes) {
  std::vector<std::pair<opentelemetry::nostd::string_view,
                        opentelemetry::common::AttributeValue>>
      result;
  result.reserve(attributes.size());
  for (const auto& [name, value] : attributes) {
    result.emplace_back(opentelemetry::nostd::string_view{name},
                        opentelemetry::nostd::string_view{value});
  }
  return result;
}

MetricAttributes MergeAttributes(const MetricAttributes& meter_attributes,
                                 const MetricAttributes& attributes) {
  MetricAttributes result = meter_attributes;
  for (const auto& [name, value] : attributes) {
    result.insert_or_assign(name, value);
  }
  return result;
}

opentelemetry::nostd::span<
    const std::pair<opentelemetry::nostd::string_view,
                    opentelemetry::common::AttributeValue>>
MakeOtelAttributeSpan(
    const std::vector<std::pair<opentelemetry::nostd::string_view,
                                opentelemetry::common::AttributeValue>>&
        attributes) {
  return {attributes.data(), attributes.size()};
}

}  // namespace

class OpenTelemetryMetrics::Impl {
 public:
  explicit Impl(OpenTelemetryMetricsOptions options) {
    otlp::OtlpGrpcMetricExporterOptions exporter_options;
    exporter_options.endpoint = NormalizeGrpcEndpoint(options.endpoint);
    exporter_options.use_ssl_credentials = false;
    exporter_options.timeout = options.export_timeout;

    auto exporter =
        otlp::OtlpGrpcMetricExporterFactory::Create(exporter_options);

    metrics_sdk::PeriodicExportingMetricReaderOptions reader_options;
    reader_options.export_interval_millis = options.export_interval;
    reader_options.export_timeout_millis = options.export_timeout;

    auto reader = metrics_sdk::PeriodicExportingMetricReaderFactory::Create(
        std::move(exporter), reader_options);

    const resource::ResourceAttributes resource_attributes = {
        {"service.name", options.service_name}};
    auto context = metrics_sdk::MeterContextFactory::Create(
        metrics_sdk::ViewRegistryFactory::Create(),
        resource::Resource::Create(resource_attributes));
    context->AddMetricReader(std::move(reader));

    auto provider =
        metrics_sdk::MeterProviderFactory::Create(std::move(context));
    provider_ = opentelemetry::nostd::shared_ptr<metrics_api::MeterProvider>(
        provider.release());
    previous_provider_ = metrics_api::Provider::GetMeterProvider();
    metrics_api::Provider::SetMeterProvider(provider_);
  }

  ~Impl() { metrics_api::Provider::SetMeterProvider(previous_provider_); }

 private:
  opentelemetry::nostd::shared_ptr<metrics_api::MeterProvider> provider_;
  opentelemetry::nostd::shared_ptr<metrics_api::MeterProvider>
      previous_provider_;
};

OpenTelemetryMetrics::OpenTelemetryMetrics(OpenTelemetryMetricsOptions options)
    : impl_{std::make_unique<Impl>(std::move(options))} {}

OpenTelemetryMetrics::~OpenTelemetryMetrics() = default;

class Meter::Impl {
 public:
  Impl(std::string meter_name, MetricAttributes attributes)
      : meter_{metrics_api::Provider::GetMeterProvider()->GetMeter(meter_name,
                                                                   "1.0.0")},
        attributes_{std::move(attributes)} {}

  void AddCounter(std::string_view metric_name,
                  std::uint64_t value,
                  const MetricAttributes& attributes) {
    auto& counter = FindOrCreateInstrument(
        counters_, mutex_, metric_name, [this](std::string_view name) {
          return meter_->CreateUInt64Counter(std::string{name});
        });
    const auto merged_attributes = MergeAttributes(attributes_, attributes);
    const auto otel_attributes = MakeOtelAttributes(merged_attributes);
    counter.Add(value, MakeOtelAttributeSpan(otel_attributes));
  }

  void AddUpDownCounter(std::string_view metric_name,
                        std::int64_t delta,
                        const MetricAttributes& attributes) {
    auto& counter = FindOrCreateInstrument(
        up_down_counters_, mutex_, metric_name, [this](std::string_view name) {
          return meter_->CreateInt64UpDownCounter(std::string{name});
        });
    const auto merged_attributes = MergeAttributes(attributes_, attributes);
    const auto otel_attributes = MakeOtelAttributes(merged_attributes);
    counter.Add(delta, MakeOtelAttributeSpan(otel_attributes));
  }

  void RecordHistogram(std::string_view metric_name,
                       double value,
                       const MetricAttributes& attributes) {
    auto& histogram = FindOrCreateInstrument(
        histograms_, mutex_, metric_name, [this](std::string_view name) {
          return meter_->CreateDoubleHistogram(std::string{name});
        });
    const auto merged_attributes = MergeAttributes(attributes_, attributes);
    const auto otel_attributes = MakeOtelAttributes(merged_attributes);
    histogram.Record(value, MakeOtelAttributeSpan(otel_attributes),
                     opentelemetry::context::Context{});
  }

 private:
  opentelemetry::nostd::shared_ptr<metrics_api::Meter> meter_;
  MetricAttributes attributes_;
  std::mutex mutex_;
  std::unordered_map<std::string,
                     std::unique_ptr<metrics_api::Counter<std::uint64_t>>>
      counters_;
  std::unordered_map<std::string,
                     std::unique_ptr<metrics_api::UpDownCounter<std::int64_t>>>
      up_down_counters_;
  std::unordered_map<std::string,
                     std::unique_ptr<metrics_api::Histogram<double>>>
      histograms_;
};

Meter::Meter(std::string meter_name) : Meter{std::move(meter_name), {}} {}

Meter::Meter(std::string meter_name, MetricAttributes attributes)
    : impl_{std::make_unique<Impl>(std::move(meter_name),
                                   std::move(attributes))} {}

Meter::~Meter() = default;

void Meter::AddCounter(std::string_view metric_name,
                       std::uint64_t value,
                       const MetricAttributes& attributes) {
  impl_->AddCounter(metric_name, value, attributes);
}

void Meter::AddUpDownCounter(std::string_view metric_name,
                             std::int64_t delta,
                             const MetricAttributes& attributes) {
  impl_->AddUpDownCounter(metric_name, delta, attributes);
}

void Meter::RecordHistogram(std::string_view metric_name,
                            double value,
                            const MetricAttributes& attributes) {
  impl_->RecordHistogram(metric_name, value, attributes);
}

}  // namespace metrics
