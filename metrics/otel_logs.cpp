#include "metrics/otel_logs.h"

#include "metrics/otel_endpoint.h"

#include <opentelemetry/exporters/otlp/otlp_grpc_log_record_exporter_factory.h>
#include <opentelemetry/exporters/otlp/otlp_grpc_log_record_exporter_options.h>
#include <opentelemetry/sdk/logs/batch_log_record_processor_factory.h>
#include <opentelemetry/sdk/logs/batch_log_record_processor_options.h>
#include <opentelemetry/sdk/logs/logger_provider.h>
#include <opentelemetry/sdk/logs/logger_provider_factory.h>
#include <opentelemetry/sdk/resource/resource.h>

#include <chrono>

namespace metrics {

namespace {

namespace otlp = opentelemetry::exporter::otlp;
namespace resource = opentelemetry::sdk::resource;
namespace logs_sdk = opentelemetry::sdk::logs;

std::unique_ptr<logs_sdk::LogRecordExporter> CreateOtlpExporter(
    const OpenTelemetryLogsOptions& options) {
  otlp::OtlpGrpcLogRecordExporterOptions exporter_options;
  exporter_options.endpoint = NormalizeGrpcEndpoint(options.endpoint);
  exporter_options.use_ssl_credentials = false;
  return otlp::OtlpGrpcLogRecordExporterFactory::Create(exporter_options);
}

}  // namespace

class OpenTelemetryLogs::Impl {
 public:
  Impl(OpenTelemetryLogsOptions options,
       std::unique_ptr<logs_sdk::LogRecordExporter> exporter) {
    auto processor = logs_sdk::BatchLogRecordProcessorFactory::Create(
        std::move(exporter), logs_sdk::BatchLogRecordProcessorOptions{});

    const resource::ResourceAttributes resource_attributes = {
        {"service.name", options.service_name}};

    provider_ = logs_sdk::LoggerProviderFactory::Create(
        std::move(processor), resource::Resource::Create(resource_attributes));
  }

  ~Impl() {
    using namespace std::chrono_literals;
    // Flush buffered records before teardown so short-lived processes
    // (tests, clean shutdowns) do not silently drop their last batch.
    provider_->ForceFlush(5s);
    provider_->Shutdown();
  }

  void ForceFlush() {
    using namespace std::chrono_literals;
    provider_->ForceFlush(5s);
  }

  opentelemetry::nostd::shared_ptr<opentelemetry::logs::Logger> logger() {
    return provider_->GetLogger("scada", "scada");
  }

 private:
  std::unique_ptr<logs_sdk::LoggerProvider> provider_;
};

OpenTelemetryLogs::OpenTelemetryLogs(OpenTelemetryLogsOptions options)
    : impl_{std::make_unique<Impl>(options, CreateOtlpExporter(options))} {}

OpenTelemetryLogs::OpenTelemetryLogs(
    OpenTelemetryLogsOptions options,
    std::unique_ptr<opentelemetry::sdk::logs::LogRecordExporter> exporter)
    : impl_{std::make_unique<Impl>(std::move(options), std::move(exporter))} {}

OpenTelemetryLogs::~OpenTelemetryLogs() = default;

opentelemetry::nostd::shared_ptr<opentelemetry::logs::Logger>
OpenTelemetryLogs::logger() {
  return impl_->logger();
}

void OpenTelemetryLogs::ForceFlush() {
  impl_->ForceFlush();
}

}  // namespace metrics
