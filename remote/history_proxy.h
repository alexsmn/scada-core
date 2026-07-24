#pragma once

#include "base/any_executor.h"
#include "scada/history_service.h"

class MessageSender;

class HistoryProxy : public scada::HistoryService {
 public:
  explicit HistoryProxy(AnyExecutor executor);

  void OnChannelOpened(MessageSender& sender);
  void OnChannelClosed();

  // scada::HistoryService
  virtual Awaitable<scada::StatusOr<scada::HistoryReadRawResult>>
  HistoryReadRaw(scada::HistoryReadRawDetails details) override;
  virtual Awaitable<scada::StatusOr<scada::HistoryReadEventsResult>>
  HistoryReadEvents(scada::NodeId node_id,
                    scada::Time from,
                    scada::Time to,
                    scada::EventFilter filter) override;

 private:
  AnyExecutor executor_;
  MessageSender* sender_ = nullptr;
};
