#pragma once

#include "base/cancelation.h"

#include <functional>

class Metrics;

class MetricService {
 public:
  virtual ~MetricService() = default;

  using Provider = std::function<void(Metrics& metrics)>;

  virtual void RegisterProvider(const CancelationRef& cancelation,
                                const Provider& provider) = 0;
};