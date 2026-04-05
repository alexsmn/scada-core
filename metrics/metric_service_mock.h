#pragma once

#include "metrics/metric_service.h"

#include <gmock/gmock.h>

class MockMetricService : public MetricService {
 public:
  MOCK_METHOD(void, RegisterProvider, (const Provider& provider), (override));

  MOCK_METHOD(void, RegisterSink, (const Sink& sink), (override));
};
