#pragma once

#include <string>
#include <string_view>

namespace scada::metrics {

// Strips an http(s):// scheme from an OTLP endpoint: the OTLP/gRPC exporters
// expect a bare host:port target. Shared by the metrics and traces runtimes.
inline std::string NormalizeGrpcEndpoint(std::string endpoint) {
  constexpr std::string_view kHttpPrefix = "http://";
  constexpr std::string_view kHttpsPrefix = "https://";

  if (endpoint.starts_with(kHttpPrefix)) {
    endpoint.erase(0, kHttpPrefix.size());
  } else if (endpoint.starts_with(kHttpsPrefix)) {
    endpoint.erase(0, kHttpsPrefix.size());
  }

  return endpoint;
}

}  // namespace scada::metrics
