#pragma once

#include "base/stream_utf.h"
#include "base/utf_convert.h"

#include <boost/log/common.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/formatting_ostream.hpp>
#include <boost/log/utility/manipulators/add_value.hpp>
#include <optional>

// The wide / UTF-16 string operator<< overloads for std::ostream live in
// base/stream_utf.h (included above). The overloads below are the
// boost::log::formatting_ostream counterparts, kept here next to the Boost.Log
// include they require; ADL finds them for that stream type even where a nearer
// operator<< shadows the global ones.
namespace boost::log {
inline formatting_ostream& operator<<(formatting_ostream& os,
                                      std::u16string_view sv) {
  os << UtfConvert<char>(sv);
  return os;
}
inline formatting_ostream& operator<<(formatting_ostream& os,
                                      const std::u16string& str) {
  os << UtfConvert<char>(std::u16string_view{str});
  return os;
}
}  // namespace boost::log

using BoostLogSeverity = boost::log::trivial::severity_level;

using BoostLogger =
    boost::log::sources::severity_channel_logger_mt<BoostLogSeverity>;

#define LOG_TRACE(logger) BOOST_LOG_SEV(logger, ::BoostLogSeverity::trace)
// Debug level is only logged in debug builds.
#define LOG_DEBUG(logger) BOOST_LOG_SEV(logger, ::BoostLogSeverity::debug)
#define LOG_INFO(logger) BOOST_LOG_SEV(logger, ::BoostLogSeverity::info)
#define LOG_WARNING(logger) BOOST_LOG_SEV(logger, ::BoostLogSeverity::warning)
#define LOG_ERROR(logger) BOOST_LOG_SEV(logger, ::BoostLogSeverity::error)
#define LOG_CRITICAL(logger) BOOST_LOG_SEV(logger, ::BoostLogSeverity::fatal)
#define LOG_SEV(logger, severity) BOOST_LOG_SEV(logger, severity)
#define LOG_STATUS(logger, status) \
  LOG_SEV(logger,                  \
          status ? ::BoostLogSeverity::info : ::BoostLogSeverity::warning)

#define LOG_TAG(name, value) boost::log::add_value(name, value)
#define LOG_SCOPED_TAG(logger, name, value) \
  BOOST_LOG_SCOPED_LOGGER_TAG(logger, name, value)
#define LOG_BIND_TAG(logger, name, value) \
  (logger).add_attribute((name), boost::log::attributes::make_constant(value))

#define LOG_NAME(name) boost::log::keywords::channel = (name)

std::optional<BoostLogSeverity> ParseLogSeverity(std::string_view str);
std::string_view ToStringView(BoostLogSeverity log_severity);
