#pragma once

#include <boost/signals2/connection.hpp>
#include <string_view>

namespace scada {

class SessionDebugger {
 public:
  using RequestId = int;
  enum class RequestPhase { Running, Succeeded, Failed };

  struct RequestEvent {
    RequestId id;
    RequestPhase phase;
    std::string_view title;
    std::string_view body;
    std::string_view response_body;
  };

  using RequestEventCallback = std::function<void(const RequestEvent& request)>;

  virtual boost::signals2::scoped_connection SubscribeRequestEvents(
      const RequestEventCallback& callback) = 0;
};

}  // namespace scada

inline std::string ToString(scada::SessionDebugger::RequestPhase phase) {
  switch (phase) {
    case scada::SessionDebugger::RequestPhase::Running:
      return "Running";
    case scada::SessionDebugger::RequestPhase::Succeeded:
      return "Succeeded";
    case scada::SessionDebugger::RequestPhase::Failed:
      return "Failed";
    default:
      return "Unknown";
  }
}
