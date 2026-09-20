#pragma once

#include "base/any_executor.h"

#include "base/awaitable.h"
#include "base/boost_log.h"
#include "metrics/tracer.h"
#include "scada/history_types.h"

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
  // `locale_ids` are the session's language preferences, passed in because
  // `HistoryService` carries no ServiceContext — a historical event's Message
  // is a LocalizedText and is localizable like any other (OPC UA Part 5
  // §6.4.2), and Part 11 states no exception for historical access, so the
  // session's language governs here too. Resolving at this endpoint rather
  // than widening the HistoryService interface keeps a sold API unchanged;
  // the same gap is why these spans do not propagate downstream.
  HistoryStub(scada::HistoryService& service,
              std::weak_ptr<MessageSender> sender,
              AnyExecutor executor,
              std::vector<std::string> locale_ids = {},
              Tracer& tracer = Tracer::None());
  ~HistoryStub();

  void OnRequestReceived(const protocol::Request& request);

 private:
  // `trace_id` is the request's traceparent (or empty). HistoryService
  // carries no ServiceContext, so these spans parent from it directly and the
  // trace does not propagate further downstream.
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
  // The session's languages, fixed for the life of the session.
  const std::vector<std::string> locale_ids_;

  BoostLogger logger_{LOG_NAME("HistoryStub")};

  std::map<scada::ByteString, scada::HistoryReadRawDetails>
      continuation_points_;
};
