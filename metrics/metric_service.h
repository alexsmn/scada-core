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
  // `BindPromiseExecutorWithResult()`.
  using Provider = std::function<promise<Metrics>()>;

  virtual void RegisterProvider(const Provider& provider) = 0;
};