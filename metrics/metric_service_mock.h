#pragma once

#include "metrics/metric_service.h"

#include <gmock/gmock.h>

class MockMetricService : public MetricService {
 public:
  MOCK_METHOD(void,
              RegisterProvider,
              (const CancelationRef& cancelation, const Provider& provider),
              (override));
};
