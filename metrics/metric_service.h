#pragma once

#include "base/awaitable.h"
#include "base/cancelation.h"
#include "metrics/metrics.h"
#include "scada/status_or.h"

#include <functional>

class Metrics;

class MetricService {
 public:
  virtual ~MetricService() = default;

  // TODO: Rename to a "source", similar to Boost.Log source/sink.
  using Provider = std::function<Awaitable<scada::StatusOr<Metrics>>()>;

  // To unsubscribe, return a bad status from the provider coroutine.
  virtual void RegisterProvider(const Provider& provider) = 0;

  using Sink = std::function<void(const Metrics& metrics)>;

  virtual void RegisterSink(const Sink& sink) = 0;
};
