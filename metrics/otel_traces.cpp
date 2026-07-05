#include "metrics/otel_traces.h"

#include "metrics/otel_endpoint.h"

#include <opentelemetry/exporters/otlp/otlp_grpc_exporter_factory.h>
#include <opentelemetry/exporters/otlp/otlp_grpc_exporter_options.h>
#include <opentelemetry/sdk/resource/resource.h>
#include <opentelemetry/sdk/trace/batch_span_processor_factory.h>
#include <opentelemetry/sdk/trace/batch_span_processor_options.h>
#include <opentelemetry/sdk/trace/samplers/parent_factory.h>
#include <opentelemetry/sdk/trace/samplers/trace_id_ratio_factory.h>
#include <opentelemetry/sdk/trace/tracer_provider.h>
#include <opentelemetry/sdk/trace/tracer_provider_factory.h>

#include <algorithm>
#include <chrono>

namespace metrics {

namespace {

namespace otlp = opentelemetry::exporter::otlp;
namespace resource = opentelemetry::sdk::resource;
namespace trace_sdk = opentelemetry::sdk::trace;

std::unique_ptr<trace_sdk::SpanExporter> CreateOtlpExporter(
    const OpenTelemetryTracesOptions& options) {
  otlp::OtlpGrpcExporterOptions exporter_options;
  exporter_options.endpoint = NormalizeGrpcEndpoint(options.endpoint);
  exporter_options.use_ssl_credentials = false;
  return otlp::OtlpGrpcExporterFactory::Create(exporter_options);
}

}  // namespace

class OpenTelemetryTraces::Impl {
 public:
  Impl(OpenTelemetryTracesOptions options,
       std::unique_ptr<trace_sdk::SpanExporter> exporter) {
    auto processor = trace_sdk::BatchSpanProcessorFactory::Create(
        std::move(exporter), trace_sdk::BatchSpanProcessorOptions{});

    auto sampler = trace_sdk::ParentBasedSamplerFactory::Create(
        trace_sdk::TraceIdRatioBasedSamplerFactory::Create(
            std::clamp(options.sampling_ratio, 0.0, 1.0)));

    const resource::ResourceAttributes resource_attributes = {
        {"service.name", options.service_name}};

    provider_ = trace_sdk::TracerProviderFactory::Create(
        std::move(processor), resource::Resource::Create(resource_attributes),
        std::move(sampler));
  }

  ~Impl() {
    using namespace std::chrono_literals;
    // Flush buffered spans before teardown so short-lived processes (tests,
    // clean shutdowns) do not silently drop their last batch.
    provider_->ForceFlush(5s);
    provider_->Shutdown();
  }

  void ForceFlush() {
    using namespace std::chrono_literals;
    provider_->ForceFlush(5s);
  }

  opentelemetry::nostd::shared_ptr<opentelemetry::trace::Tracer> tracer() {
    return opentelemetry::nostd::shared_ptr<opentelemetry::trace::Tracer>{
        provider_->GetTracer("scada")};
  }

 private:
  std::unique_ptr<trace_sdk::TracerProvider> provider_;
};

OpenTelemetryTraces::OpenTelemetryTraces(OpenTelemetryTracesOptions options)
    : impl_{std::make_unique<Impl>(options, CreateOtlpExporter(options))} {}

OpenTelemetryTraces::OpenTelemetryTraces(
    OpenTelemetryTracesOptions options,
    std::unique_ptr<opentelemetry::sdk::trace::SpanExporter> exporter)
    : impl_{std::make_unique<Impl>(std::move(options), std::move(exporter))} {}

OpenTelemetryTraces::~OpenTelemetryTraces() = default;

opentelemetry::nostd::shared_ptr<opentelemetry::trace::Tracer>
OpenTelemetryTraces::tracer() {
  return impl_->tracer();
}

void OpenTelemetryTraces::ForceFlush() {
  impl_->ForceFlush();
}

}  // namespace metrics
