#pragma once

#include "base/boost_log.h"
#include "base/awaitable.h"
#include "scada/history_types.h"

#include <map>

namespace boost::asio {
class io_context;
}

namespace protocol {
class Request;
}

namespace scada {
class HistoryService;
}

class Executor;
class MessageSender;

class HistoryStub : public std::enable_shared_from_this<HistoryStub> {
 public:
  HistoryStub(scada::HistoryService& service,
              std::weak_ptr<MessageSender> sender,
              std::shared_ptr<Executor> executor);
  ~HistoryStub();

  void OnRequestReceived(const protocol::Request& request);

 private:
  void OnHistoryReadRaw(const protocol::Request& request);
  void OnHistoryReadEvents(const protocol::Request& request);
  [[nodiscard]] Awaitable<void> OnHistoryReadRawAsync(
      unsigned request_id,
      scada::HistoryReadRawDetails details);
  [[nodiscard]] Awaitable<void> OnHistoryReadEventsAsync(
      unsigned request_id,
      scada::NodeId node_id,
      base::Time from,
      base::Time to,
      scada::EventFilter filter);

  scada::HistoryService& service_;
  const std::weak_ptr<MessageSender> sender_;
  const std::shared_ptr<Executor> executor_;

  BoostLogger logger_{LOG_NAME("HistoryStub")};

  std::map<scada::ByteString, scada::HistoryReadRawDetails>
      continuation_points_;
};
