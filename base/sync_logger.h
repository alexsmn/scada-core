#pragma once

#include "base/logger.h"

#include <boost/asio/io_context.hpp>

class SyncLogger final : public Logger {
 public:
  SyncLogger(std::shared_ptr<const Logger> logger,
             boost::asio::io_context& io_context)
      : logger_{std::move(logger)}, io_context_{io_context} {}

  void Write(LogSeverity severity, std::string_view message) const override {
    io_context_.dispatch(
        [logger = logger_, severity, copied_message = std::string{message}] {
          logger->Write(severity, copied_message);
        });
  }

 private:
  const std::shared_ptr<const Logger> logger_;
  boost::asio::io_context& io_context_;
};
