#pragma once

#include "base/any_executor.h"

#include "base/awaitable.h"
#include "base/boost_log.h"
#include "metrics/tracer.h"
#include "scada/history_types.h"
#include "scada/service_context.h"

#include <map>
#include <string>
#include <vector>

namespace boost::asio {
class io_context;
}

namespace protocol {
class Request;
}

namespace scada {
class HistoryService;
}

class MessageSender;

class HistoryStub : public std::enable_shared_from_this<HistoryStub> {
 public:
  // `service_context` is the session's caller. HistoryReadEvents hands it to
  // the service, which withholds the events the caller may not receive (OPC
  // UA Part 3 §8.55 PermissionType bit 11 ReceiveEvents,
  // https://reference.opcfoundation.org/Core/Part3/v105/docs/8.55). Its
  // locale_ids also resolve each event's Message here: a historical event's
  // Message is a LocalizedText and is localizable like any other (OPC UA
  // Part 5 §6.4.2), and Part 11 states no exception for historical access,
  // so the session's language governs here too. HistoryReadRaw still carries
  // no ServiceContext, so its spans do not propagate downstream.
  HistoryStub(scada::HistoryService& service,
              std::weak_ptr<MessageSender> sender,
              AnyExecutor executor,
              scada::ServiceContext service_context = {},
              Tracer& tracer = Tracer::None());
  ~HistoryStub();

  void OnRequestReceived(const protocol::Request& request);

 private:
  // `trace_id` is the request's traceparent (or empty). These spans parent
  // from it directly.
  void OnHistoryReadRaw(const protocol::Request& request);
  void OnHistoryReadEvents(const protocol::Request& request);
  [[nodiscard]] Awaitable<void> OnHistoryReadRawAsync(
      unsigned request_id,
      std::string trace_id,
      scada::HistoryReadRawDetails details);
  [[nodiscard]] Awaitable<void> OnHistoryReadEventsAsync(
      unsigned request_id,
      std::string trace_id,
      scada::NodeId node_id,
      scada::Time from,
      scada::Time to,
      scada::EventFilter filter);

  scada::HistoryService& service_;
  const std::weak_ptr<MessageSender> sender_;
  const AnyExecutor executor_;
  Tracer& tracer_;
  // The session's caller, fixed for the life of the session.
  const scada::ServiceContext service_context_;
  // The session's languages (service_context_.locale_ids()).
  const std::vector<std::string> locale_ids_;

  BoostLogger logger_{LOG_NAME("HistoryStub")};

  std::map<scada::ByteString, scada::HistoryReadRawDetails>
      continuation_points_;
};
