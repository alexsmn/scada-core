#include "base/boost_log_init.h"

#include "base/boost_log.h"
#include "base/format.h"

#include <boost/algorithm/string.hpp>
#include <boost/date_time/posix_time/time_formatters.hpp>
#include <boost/filesystem.hpp>
#include "base/utf_convert.h"
#include <boost/log/attributes.hpp>
#include <boost/log/attributes/clock.hpp>
#include <boost/log/attributes/value_visitation.hpp>
#include <boost/log/utility/functional/save_result.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <string>

namespace {

BOOST_LOG_ATTRIBUTE_KEYWORD(severity, "Severity", BoostLogSeverity)

constexpr std::pair<BoostLogSeverity, std::string_view> kLogSeverities[] = {
    {BoostLogSeverity::trace, "Trace"}, {BoostLogSeverity::debug, "Debug"},
    {BoostLogSeverity::info, "Info"},   {BoostLogSeverity::warning, "Warning"},
    {BoostLogSeverity::error, "Error"}, {BoostLogSeverity::fatal, "Critical"}};

// GCC requires non-empty namespace for explicit specialization.
template <class T>
std::string EvaluateString(const T& value) {
  return Format(value);
}

template <>
std::string EvaluateString(const bool& value) {
  return Format(value ? 1 : 0);
}

template <>
std::string EvaluateString(const std::wstring& value) {
  return UtfConvert<char>(value);
}

template <>
std::string EvaluateString(const std::u16string& value) {
  return UtfConvert<char>(value);
}

std::string ToString(const boost::log::attribute_value& attr) {
  // NOTE: It's not clear why long is required at least under Windows.
  using Types = boost::mpl::vector<bool, int16_t, uint16_t, int32_t, uint32_t,
                                   int64_t, uint64_t, long, float, double,
                                   std::string, std::wstring, std::u16string>;

  std::string result;
  boost::log::visit<Types>(
      attr,
      boost::log::save_result(
          [](const auto& value) { return EvaluateString(value); }, result));

  return result;
}

void FormatLogRecord(const boost::log::record_view& record,
                     bool console,
                     boost::log::formatting_ostream& stream) {
  auto severity = BoostLogSeverity::info;
  std::string channel;
  std::string line_id;
  using TimeStamp = boost::log::attributes::local_clock::value_type;
  TimeStamp timestamp;
  std::string message;

  std::vector<std::pair<const boost::log::attribute_name*, std::string>> attrs;
  attrs.reserve(record.attribute_values().size());

  for (auto& attr : record.attribute_values()) {
    if (attr.first == boost::log::aux::default_attribute_names::severity()) {
      severity = attr.second.extract_or_default(BoostLogSeverity::info);
      continue;
    } else if (attr.first ==
               boost::log::aux::default_attribute_names::timestamp()) {
      timestamp = attr.second.extract_or_default(TimeStamp());
      continue;
    }

    auto string_value = ToString(attr.second);
    if (!string_value.empty()) {
      if (attr.first == boost::log::aux::default_attribute_names::channel())
        channel = std::move(string_value);
      else if (attr.first ==
               boost::log::aux::default_attribute_names::line_id())
        line_id = std::move(string_value);
      else if (attr.first ==
               boost::log::aux::default_attribute_names::message())
        message = std::move(string_value);
      else
        attrs.emplace_back(&attr.first, std::move(string_value));
    }
  }

  stream << "#" << line_id << " ";

  if (!console)
    stream << "[" << boost::posix_time::to_simple_string(timestamp) << "] ";

  stream << "[" << ToStringView(severity) << "] ";

  if (!channel.empty())
    stream << channel << ": ";

  stream << message;

  for (auto& attr : attrs)
    stream << " | " << *attr.first << " = " << attr.second;
}

template <bool kConsole>
void FormatLogRecordT(const boost::log::record_view& record,
                      boost::log::formatting_ostream& stream) {
  FormatLogRecord(record, kConsole, stream);
}

std::filesystem::path GetLogFileName(const std::filesystem::path& path) {
  auto new_path = path;
  auto suffix = "_%Y-%m-%d_%H-%M-%S-%N" + new_path.extension().string();
  new_path.replace_extension();
  new_path += suffix;
  return new_path;
}

std::filesystem::path GetLogTarget(const std::filesystem::path& path) {
  return path.parent_path();
}

}  // namespace

std::optional<BoostLogSeverity> ParseLogSeverity(std::string_view str) {
  auto i = std::ranges::find_if(
      kLogSeverities, [str](auto&& x) { return boost::iequals(str, x); },
      [](auto&& p) { return p.second; });
  return i == std::end(kLogSeverities)
             ? std::optional<BoostLogSeverity>{}
             : std::optional<BoostLogSeverity>{i->first};
}

std::string_view ToStringView(BoostLogSeverity severity) {
  auto i = std::ranges::find(kLogSeverities, severity,
                             [](auto&& p) { return p.first; });
  return i == std::end(kLogSeverities) ? "Info" : i->second;
}

void InitBoostLogging(const BoostLogParams& params) {
  static bool attributes_installed = false;
  if (!attributes_installed) {
    boost::log::add_common_attributes();
    attributes_installed = true;
  }

  static bool console_installed = false;
  if (!console_installed && params.console) {
    auto sink = boost::log::add_console_log();
    boost::log::core::get()->add_sink(sink);
    sink->set_formatter(&FormatLogRecordT<true>);
    sink->set_filter(severity >= params.console_log_severity);
    console_installed = true;
  }

  static bool file_installed = false;
  if (!file_installed && !params.path.empty()) {
    try {
      auto sink = boost::log::add_file_log(
          boost::log::keywords::file_name = GetLogFileName(params.path),
          boost::log::keywords::target = GetLogTarget(params.path),
          boost::log::keywords::rotation_size = params.rotation_size,
          boost::log::keywords::max_size = params.max_size,
          boost::log::keywords::max_files = params.max_files,
      // Build fails in WSL.
#if defined(WIN32)
          boost::log::keywords::format =
              "%TimeStamp% (%LineID%) <%Severity%>: %Message%",
#endif
          boost::log::keywords::time_based_rotation =
              boost::log::sinks::file::rotation_at_time_point(0, 0, 0));
      sink->set_formatter(&FormatLogRecordT<false>);
      sink->locked_backend()->auto_flush();

      file_installed = true;

    } catch (const boost::filesystem::filesystem_error&) {
    }
  }
}

void ShutdownBoostLogging() {
  if (const auto& core = boost::log::core::get())
    core->remove_all_sinks();
}
