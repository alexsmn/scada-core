#include "metrics/structured_log_formatter.h"

#include "base/utf_convert.h"
#include "metrics/boost_log_attribute_types.h"
#include "metrics/otel_log_sink.h"
#include "metrics/trace_parent.h"

#include <boost/json.hpp>
#include <boost/log/attributes/value_visitation.hpp>

#include <chrono>
#include <ctime>
#include <format>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <type_traits>

namespace scada::metrics {

namespace {

// Google Cloud LogEntry severity names
// (https://cloud.google.com/logging/docs/reference/v2/rest/v2/LogEntry#LogSeverity).
// Cloud Logging has no TRACE level, so Boost trace maps to DEBUG.
std::string_view ToGoogleSeverity(BoostLogSeverity severity) {
  switch (severity) {
    case BoostLogSeverity::trace:
    case BoostLogSeverity::debug:
      return "DEBUG";
    case BoostLogSeverity::info:
      return "INFO";
    case BoostLogSeverity::warning:
      return "WARNING";
    case BoostLogSeverity::error:
      return "ERROR";
    case BoostLogSeverity::fatal:
      return "CRITICAL";
  }
  return "INFO";
}

// RFC 3339 UTC with microseconds, e.g. "2026-07-06T12:34:56.789012Z".
std::string FormatRfc3339Now() {
  const auto now = std::chrono::system_clock::now();
  const std::time_t seconds = std::chrono::system_clock::to_time_t(now);
  const auto microseconds =
      std::chrono::duration_cast<std::chrono::microseconds>(
          now.time_since_epoch()) %
      std::chrono::seconds{1};

  std::tm utc{};
#if defined(WIN32)
  gmtime_s(&utc, &seconds);
#else
  gmtime_r(&seconds, &utc);
#endif
  return std::format("{:04}-{:02}-{:02}T{:02}:{:02}:{:02}.{:06}Z",
                     utc.tm_year + 1900, utc.tm_mon + 1, utc.tm_mday,
                     utc.tm_hour, utc.tm_min, utc.tm_sec, microseconds.count());
}

std::string ToLowerHex(std::span<const uint8_t> bytes) {
  std::string result;
  result.reserve(bytes.size() * 2);
  for (uint8_t byte : bytes)
    result += std::format("{:02x}", byte);
  return result;
}

// Adds one Boost.Log attribute value as a typed JSON field. emplace() keeps
// an existing key, so the reserved fields inserted before the attribute walk
// (severity/time/message/...) cannot be clobbered by same-named LOG_TAGs.
void EmplaceTypedAttribute(boost::json::object& object,
                           std::string_view key,
                           const boost::log::attribute_value& attr) {
  boost::log::visit<BoostLogAttributeTypes>(attr, [&](const auto& value) {
    using T = std::decay_t<decltype(value)>;
    if constexpr (std::is_same_v<T, bool>) {
      object.emplace(key, value);
    } else if constexpr (std::is_same_v<T, std::string>) {
      object.emplace(key, value);
    } else if constexpr (std::is_same_v<T, std::wstring> ||
                         std::is_same_v<T, std::u16string>) {
      object.emplace(key, UtfConvert<char>(value));
    } else if constexpr (std::is_floating_point_v<T>) {
      object.emplace(key, static_cast<double>(value));
    } else if constexpr (std::is_signed_v<T>) {
      object.emplace(key, static_cast<int64_t>(value));
    } else {
      object.emplace(key, static_cast<uint64_t>(value));
    }
  });
}

void FormatRecord(const StructuredLogFormatterOptions& options,
                  const boost::log::record_view& record,
                  boost::log::formatting_ostream& stream) {
  auto severity = BoostLogSeverity::info;
  std::string channel;
  std::string message;
  std::optional<W3CTraceParent> trace_parent;

  boost::json::object object;

  namespace names = boost::log::aux::default_attribute_names;
  for (const auto& attr : record.attribute_values()) {
    if (attr.first == names::severity()) {
      severity = attr.second.extract_or_default(BoostLogSeverity::info);
    } else if (attr.first == names::timestamp()) {
      // Skipped: the Boost attribute is a local-clock ptime; the RFC 3339
      // UTC stamp below is exact because formatting runs at the emit site.
    } else if (attr.first == names::channel()) {
      channel = attr.second.extract_or_default(std::string{});
    } else if (attr.first == names::message()) {
      message = attr.second.extract_or_default(std::string{});
    } else if (attr.first.string() == kTraceParentLogAttribute) {
      trace_parent =
          ParseTraceParent(attr.second.extract_or_default(std::string{}));
    } else {
      EmplaceTypedAttribute(object, attr.first.string(), attr.second);
    }
  }

  // Reserved convention fields. Inserted with operator[] so they win over
  // any same-named attribute emplaced above.
  object["severity"] = ToGoogleSeverity(severity);
  object["time"] = FormatRfc3339Now();
  object["message"] = message;
  if (!channel.empty())
    object["channel"] = channel;

  if (trace_parent) {
    object["logging.googleapis.com/trace"] =
        options.trace_prefix + ToLowerHex(trace_parent->trace_id);
    object["logging.googleapis.com/spanId"] = ToLowerHex(trace_parent->span_id);
    object["logging.googleapis.com/trace_sampled"] = trace_parent->sampled();
  }

  // boost::json::serialize emits no newlines, so the entry stays one line
  // (the sink backend appends the record separator).
  stream << boost::json::serialize(object);
}

}  // namespace

BoostLogFormatter MakeStructuredLogFormatter(
    StructuredLogFormatterOptions options) {
  return
      [options = std::move(options)](const boost::log::record_view& record,
                                     boost::log::formatting_ostream& stream) {
        FormatRecord(options, record, stream);
      };
}

}  // namespace scada::metrics
