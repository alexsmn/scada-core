#include "metrics/otel_log_sink.h"

#include "base/utf_convert.h"
#include "metrics/boost_log_attribute_types.h"
#include "metrics/trace_parent.h"

#include <boost/log/attributes/value_visitation.hpp>
#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/trivial.hpp>
#include <opentelemetry/common/timestamp.h>
#include <opentelemetry/logs/log_record.h>
#include <opentelemetry/logs/severity.h>
#include <opentelemetry/nostd/span.h>
#include <opentelemetry/nostd/string_view.h>
#include <opentelemetry/trace/span_id.h>
#include <opentelemetry/trace/trace_flags.h>
#include <opentelemetry/trace/trace_id.h>

#include <chrono>
#include <cstdint>
#include <optional>
#include <string>
#include <type_traits>

namespace scada::metrics {

namespace {

namespace logs_api = opentelemetry::logs;
namespace nostd = opentelemetry::nostd;

logs_api::Severity ToOtelSeverity(BoostLogSeverity severity) {
  switch (severity) {
    case BoostLogSeverity::trace:
      return logs_api::Severity::kTrace;
    case BoostLogSeverity::debug:
      return logs_api::Severity::kDebug;
    case BoostLogSeverity::info:
      return logs_api::Severity::kInfo;
    case BoostLogSeverity::warning:
      return logs_api::Severity::kWarn;
    case BoostLogSeverity::error:
      return logs_api::Severity::kError;
    case BoostLogSeverity::fatal:
      return logs_api::Severity::kFatal;
  }
  return logs_api::Severity::kInfo;
}

// Sets one Boost.Log attribute value on the OTel record with its type
// preserved. Both production and test recordables copy the value inside
// SetAttribute (OTLP serializes into protobuf, ReadWriteLogRecord stores
// OwnedAttributeValue), so passing views of locals is safe.
void SetTypedAttribute(logs_api::LogRecord& log_record,
                       nostd::string_view key,
                       const boost::log::attribute_value& attr) {
  boost::log::visit<BoostLogAttributeTypes>(attr, [&](const auto& value) {
    using T = std::decay_t<decltype(value)>;
    if constexpr (std::is_same_v<T, bool>) {
      log_record.SetAttribute(key, value);
    } else if constexpr (std::is_same_v<T, std::string>) {
      log_record.SetAttribute(key, nostd::string_view{value});
    } else if constexpr (std::is_same_v<T, std::wstring> ||
                         std::is_same_v<T, std::u16string>) {
      const std::string utf8 = UtfConvert<char>(value);
      log_record.SetAttribute(key, nostd::string_view{utf8});
    } else if constexpr (std::is_floating_point_v<T>) {
      log_record.SetAttribute(key, static_cast<double>(value));
    } else if constexpr (std::is_signed_v<T>) {
      log_record.SetAttribute(key, static_cast<int64_t>(value));
    } else {
      log_record.SetAttribute(key, static_cast<uint64_t>(value));
    }
  });
}

}  // namespace

OtelLogSinkBackend::OtelLogSinkBackend(
    opentelemetry::nostd::shared_ptr<opentelemetry::logs::Logger> logger)
    : logger_{std::move(logger)} {}

void OtelLogSinkBackend::consume(const boost::log::record_view& record) {
  auto log_record = logger_->CreateLogRecord();
  if (!log_record)
    return;

  auto severity = BoostLogSeverity::info;
  std::string channel;
  std::string message;
  std::optional<W3CTraceParent> trace_parent;

  namespace names = boost::log::aux::default_attribute_names;
  for (const auto& attr : record.attribute_values()) {
    if (attr.first == names::severity()) {
      severity = attr.second.extract_or_default(BoostLogSeverity::info);
    } else if (attr.first == names::timestamp()) {
      // Skipped: the Boost attribute is a local-clock ptime (see the class
      // comment); system_clock::now() below is exact at the emit site.
    } else if (attr.first == names::channel()) {
      channel = attr.second.extract_or_default(std::string{});
    } else if (attr.first == names::message()) {
      message = attr.second.extract_or_default(std::string{});
    } else if (attr.first.string() == kTraceParentLogAttribute) {
      trace_parent =
          ParseTraceParent(attr.second.extract_or_default(std::string{}));
    } else {
      SetTypedAttribute(*log_record, attr.first.string(), attr.second);
    }
  }

  const opentelemetry::common::SystemTimestamp now{
      std::chrono::system_clock::now()};
  log_record->SetTimestamp(now);
  log_record->SetObservedTimestamp(now);
  log_record->SetSeverity(ToOtelSeverity(severity));
  log_record->SetBody(nostd::string_view{message});
  if (!channel.empty())
    log_record->SetAttribute("channel", nostd::string_view{channel});

  if (trace_parent) {
    log_record->SetTraceId(opentelemetry::trace::TraceId{
        nostd::span<const uint8_t, opentelemetry::trace::TraceId::kSize>{
            trace_parent->trace_id.data(), trace_parent->trace_id.size()}});
    log_record->SetSpanId(opentelemetry::trace::SpanId{
        nostd::span<const uint8_t, opentelemetry::trace::SpanId::kSize>{
            trace_parent->span_id.data(), trace_parent->span_id.size()}});
    log_record->SetTraceFlags(
        opentelemetry::trace::TraceFlags{trace_parent->flags});
  }

  logger_->EmitLogRecord(std::move(log_record));
}

ScopedOtelLogSink::ScopedOtelLogSink(
    opentelemetry::nostd::shared_ptr<opentelemetry::logs::Logger> logger,
    BoostLogSeverity min_severity)
    : sink_{boost::make_shared<
          boost::log::sinks::unlocked_sink<OtelLogSinkBackend>>(
          boost::make_shared<OtelLogSinkBackend>(std::move(logger)))} {
  // The channel/severity loggers use the same "Severity" attribute as the
  // trivial keyword, so the standard expression filters on it directly.
  sink_->set_filter(boost::log::trivial::severity >= min_severity);
  boost::log::core::get()->add_sink(sink_);
}

ScopedOtelLogSink::~ScopedOtelLogSink() {
  boost::log::core::get()->remove_sink(sink_);
}

}  // namespace scada::metrics
