#include "metrics/otel_metrics.h"

#include "base/map_util.h"

#include <opentelemetry/exporters/memory/in_memory_metric_data.h>
#include <opentelemetry/exporters/memory/in_memory_metric_exporter_factory.h>
#include <opentelemetry/exporters/otlp/otlp_grpc_metric_exporter_factory.h>
#include <opentelemetry/exporters/otlp/otlp_grpc_metric_exporter_options.h>
#include <opentelemetry/metrics/provider.h>
#include <opentelemetry/sdk/instrumentationscope/instrumentation_scope.h>
#include <opentelemetry/sdk/metrics/export/periodic_exporting_metric_reader_factory.h>
#include <opentelemetry/sdk/metrics/meter_context_factory.h>
#include <opentelemetry/sdk/metrics/meter_provider_factory.h>
#include <opentelemetry/sdk/metrics/view/view_registry_factory.h>
#include <opentelemetry/sdk/resource/resource.h>

#include <boost/signals2/signal.hpp>

#include <mutex>
#include <vector>

namespace metrics {

namespace {

namespace metrics_api = opentelemetry::metrics;
namespace instrumentation = opentelemetry::sdk::instrumentationscope;
namespace metrics_sdk = opentelemetry::sdk::metrics;
namespace otlp = opentelemetry::exporter::otlp;
namespace memory = opentelemetry::exporter::memory;
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

std::optional<std::string> ReadStringAttribute(
    const metrics_sdk::PointAttributes& attributes,
    std::string_view attribute_name) {
  auto value = Find(attributes, std::string{attribute_name});
  if (!value)
    return std::nullopt;

  const auto* string_value = opentelemetry::nostd::get_if<std::string>(&*value);
  if (!string_value)
    return std::nullopt;

  return *string_value;
}

MetricValue ToMetricValue(const metrics_sdk::ValueType& value) {
  if (const auto* int_value = opentelemetry::nostd::get_if<int64_t>(&value))
    return *int_value;
  return *opentelemetry::nostd::get_if<double>(&value);
}

std::optional<MetricValue> ToMetricValue(
    const metrics_sdk::PointType& point_data) {
  if (const auto* sum =
          opentelemetry::nostd::get_if<metrics_sdk::SumPointData>(&point_data))
    return ToMetricValue(sum->value_);

  if (const auto* last_value =
          opentelemetry::nostd::get_if<metrics_sdk::LastValuePointData>(
              &point_data)) {
    if (!last_value->is_lastvalue_valid_)
      return std::nullopt;
    return ToMetricValue(last_value->value_);
  }

  if (const auto* histogram =
          opentelemetry::nostd::get_if<metrics_sdk::HistogramPointData>(
              &point_data)) {
    return static_cast<std::int64_t>(histogram->count_);
  }

  if (const auto* histogram = opentelemetry::nostd::get_if<
          metrics_sdk::Base2ExponentialHistogramPointData>(&point_data)) {
    return static_cast<std::int64_t>(histogram->count_);
  }

  return std::nullopt;
}

MetricAttributes ToMetricAttributes(
    const metrics_sdk::PointAttributes& attributes) {
  MetricAttributes result;
  for (const auto& [name, value] : attributes) {
    if (const auto* string_value =
            opentelemetry::nostd::get_if<std::string>(&value)) {
      result.try_emplace(name, *string_value);
    }
  }
  return result;
}

class ObservingInMemoryMetricData final : public memory::InMemoryMetricData {
 public:
  using Signal = boost::signals2::signal<void(const MetricPoint& point)>;

  void Add(
      std::unique_ptr<metrics_sdk::ResourceMetrics> resource_metrics) override {
    std::vector<MetricPoint> points;
    for (const auto& scope_metrics : resource_metrics->scope_metric_data_) {
      const instrumentation::InstrumentationScope* scope = scope_metrics.scope_;
      const std::string scope_name = scope ? scope->GetName() : std::string{};
      for (const auto& metric_data : scope_metrics.metric_data_) {
        for (const auto& point_data : metric_data.point_data_attr_) {
          auto value = ToMetricValue(point_data.point_data);
          if (!value)
            continue;
          points.emplace_back(MetricPoint{
              .scope_name = scope_name,
              .metric_name = metric_data.instrument_descriptor.name_,
              .attributes = ToMetricAttributes(point_data.attributes),
              .value = *value});
        }
      }
    }

    {
      std::lock_guard lock{mutex_};
      aggregate_.Add(std::move(resource_metrics));
    }

    for (const auto& point : points) {
      signal_(point);
    }
  }

  std::optional<MetricValue> GetMetricValue(std::string_view scope_name,
                                            std::string_view metric_name,
                                            std::string_view attribute_name,
                                            std::string_view attribute_value) {
    std::lock_guard lock{mutex_};
    const auto& points =
        aggregate_.Get(std::string{scope_name}, std::string{metric_name});
    for (const auto& [attributes, point_data] : points) {
      if (ReadStringAttribute(attributes, attribute_name) == attribute_value)
        return ToMetricValue(point_data);
    }
    return std::nullopt;
  }

  boost::signals2::connection Connect(MetricValueObserver observer) {
    return signal_.connect(std::move(observer));
  }

 private:
  std::mutex mutex_;
  memory::SimpleAggregateInMemoryMetricData aggregate_;
  Signal signal_;
};

}  // namespace

class OpenTelemetryMetrics::Impl {
 public:
  explicit Impl(OpenTelemetryMetricsOptions options)
      : in_memory_data_{std::make_shared<ObservingInMemoryMetricData>()} {
    otlp::OtlpGrpcMetricExporterOptions exporter_options;
    exporter_options.endpoint = NormalizeGrpcEndpoint(options.endpoint);
    exporter_options.use_ssl_credentials = false;
    exporter_options.timeout = options.export_timeout;

    auto exporter =
        otlp::OtlpGrpcMetricExporterFactory::Create(exporter_options);

    metrics_sdk::PeriodicExportingMetricReaderOptions reader_options;
    reader_options.export_interval_millis = options.export_interval;
    reader_options.export_timeout_millis = options.export_timeout;

    auto otlp_reader =
        metrics_sdk::PeriodicExportingMetricReaderFactory::Create(
            std::move(exporter), reader_options);
    auto in_memory_exporter =
        memory::InMemoryMetricExporterFactory::Create(in_memory_data_);
    auto in_memory_reader =
        metrics_sdk::PeriodicExportingMetricReaderFactory::Create(
            std::move(in_memory_exporter), reader_options);

    const resource::ResourceAttributes resource_attributes = {
        {"service.name", options.service_name}};
    auto context = metrics_sdk::MeterContextFactory::Create(
        metrics_sdk::ViewRegistryFactory::Create(),
        resource::Resource::Create(resource_attributes));
    context->AddMetricReader(std::move(otlp_reader));
    context->AddMetricReader(std::move(in_memory_reader));

    auto provider =
        metrics_sdk::MeterProviderFactory::Create(std::move(context));
    provider_ = opentelemetry::nostd::shared_ptr<metrics_api::MeterProvider>(
        provider.release());
    previous_provider_ = metrics_api::Provider::GetMeterProvider();
    metrics_api::Provider::SetMeterProvider(provider_);
  }

  ~Impl() { metrics_api::Provider::SetMeterProvider(previous_provider_); }

  std::optional<MetricValue> GetMetricValue(std::string_view scope_name,
                                            std::string_view metric_name,
                                            std::string_view attribute_name,
                                            std::string_view attribute_value) {
    return in_memory_data_->GetMetricValue(scope_name, metric_name,
                                           attribute_name, attribute_value);
  }

  boost::signals2::connection AddMetricValueObserver(
      MetricValueObserver observer) {
    return in_memory_data_->Connect(std::move(observer));
  }

 private:
  std::shared_ptr<ObservingInMemoryMetricData> in_memory_data_;
  opentelemetry::nostd::shared_ptr<metrics_api::MeterProvider> provider_;
  opentelemetry::nostd::shared_ptr<metrics_api::MeterProvider>
      previous_provider_;
};

OpenTelemetryMetrics::OpenTelemetryMetrics(OpenTelemetryMetricsOptions options)
    : impl_{std::make_unique<Impl>(std::move(options))} {}

OpenTelemetryMetrics::~OpenTelemetryMetrics() = default;

std::optional<MetricValue> OpenTelemetryMetrics::GetMetricValue(
    std::string_view scope_name,
    std::string_view metric_name,
    std::string_view attribute_name,
    std::string_view attribute_value) {
  return impl_->GetMetricValue(scope_name, metric_name, attribute_name,
                               attribute_value);
}

boost::signals2::connection OpenTelemetryMetrics::AddMetricValueObserver(
    MetricValueObserver observer) {
  return impl_->AddMetricValueObserver(std::move(observer));
}

}  // namespace metrics
