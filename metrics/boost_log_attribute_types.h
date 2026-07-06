#pragma once

#include <boost/mpl/vector.hpp>

#include <cstdint>
#include <string>

namespace metrics {

// The Boost.Log attribute value types the structured sinks extract — the
// same list the text formatter visits (boost_log_init.cpp ToString), so all
// sinks agree on which attributes are representable. Attributes of any other
// type are skipped, matching the text sinks' behavior (e.g. ProcessID /
// ThreadID never reach the output). Shared by the OTel log bridge
// (otel_log_sink.cpp) and the JSON console formatter
// (structured_log_formatter.cpp).
using BoostLogAttributeTypes = boost::mpl::vector<bool,
                                                  int16_t,
                                                  uint16_t,
                                                  int32_t,
                                                  uint32_t,
                                                  int64_t,
                                                  uint64_t,
                                                  long,
                                                  float,
                                                  double,
                                                  std::string,
                                                  std::wstring,
                                                  std::u16string>;

}  // namespace metrics
