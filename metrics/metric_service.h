#pragma once

#include "base/cancelation.h"
#include "base/promise.h"
#include "metrics/metrics.h"

#include <functional>

class Metrics;

class MetricService {
 public:
  virtual ~MetricService() = default;

  // Returning a rejected promise stops polling. It's handled automatically by
  // `BindPromiseExecutor()`.
  // TODO: Rename to a "source", similar to Boost.Log source/sink.
  using Provider = std::function<promise<Metrics>()>;

  // To unsubscribe, return a canceled promise.
  virtual void RegisterProvider(const Provider& provider) = 0;

  using Sink = std::function<void(const Metrics& metrics)>;

  virtual void RegisterSink(const Sink& sink) = 0;
};