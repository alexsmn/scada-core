#pragma once

#include "base/awaitable.h"
#include "base/cancelation.h"
#include "metrics/metrics.h"

#include <functional>

class Metrics;

class MetricService {
 public:
  virtual ~MetricService() = default;

  // Throwing from the coroutine stops polling.
  // TODO: Rename to a "source", similar to Boost.Log source/sink.
  using Provider = std::function<Awaitable<Metrics>()>;

  // To unsubscribe, throw from the provider coroutine.
  virtual void RegisterProvider(const Provider& provider) = 0;

  using Sink = std::function<void(const Metrics& metrics)>;

  virtual void RegisterSink(const Sink& sink) = 0;
};
