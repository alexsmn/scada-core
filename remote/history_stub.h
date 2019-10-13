#pragma once

#include "base/boost_log.h"
#include "core/history_types.h"

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

class MessageSender;

class HistoryStub : public std::enable_shared_from_this<HistoryStub> {
 public:
  HistoryStub(scada::HistoryService& service,
              MessageSender& sender,
              boost::asio::io_context& io_context);
  ~HistoryStub();

  void OnRequestReceived(const protocol::Request& request);

 private:
  void OnHistoryReadRaw(const protocol::Request& request);
  void OnHistoryReadEvents(const protocol::Request& request);

  scada::HistoryService& service_;
  MessageSender& sender_;
  boost::asio::io_context& io_context_;

  BoostLogger logger_{LOG_NAME("HistoryStub")};

  std::map<scada::ByteString, scada::HistoryReadRawDetails>
      continuation_points_;
};
