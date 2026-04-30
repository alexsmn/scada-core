#include "metrics/metric_service_impl.h"

#include "base/awaitable.h"
#include "base/test/asio_test_environment.h"

#include <gmock/gmock.h>
#include <stdexcept>

using namespace testing;

class MetricServiceTest : public Test {
 public:
  MetricServiceTest() {
    metric_service_.RegisterSink(metric_sink_.AsStdFunction());
  }

 protected:
  void PumpUntil(const std::function<bool()>& done) {
    for (int i = 0; i < 100 && !done(); ++i) {
      asio_env_.RunOneReadyOrBlockFor(std::chrono::milliseconds{1});
    }
    asio_env_.Poll();
  }

  Metrics MakeMetrics() {
    Metrics metrics;
    metrics.Set(kMetricName, kMetricValue);
    return metrics;
  }

  AsioTestEnvironment asio_env_;

  StrictMock<MockFunction<Awaitable<Metrics>()>> metric_provider_;
  StrictMock<MockFunction<void(const Metrics&)>> metric_sink_;

  MetricServiceImpl metric_service_{asio_env_.io_context.get_executor(),
                                    kReportPeriod};

  inline static const std::chrono::milliseconds kReportPeriod{1};
  inline static const char kMetricName[] = "Metric";
  inline static const int kMetricValue = 42;
};

TEST_F(MetricServiceTest, DoesntTriggerMetricReporterByDefault) {}

TEST_F(MetricServiceTest, WhenNoMetricsWereCollected_ReportsEmptyMetrics) {
  bool reported = false;
  EXPECT_CALL(metric_provider_, Call())
      .WillOnce([]() -> Awaitable<Metrics> { co_return Metrics{}; });

  EXPECT_CALL(metric_sink_, Call(Property(&Metrics::empty, IsTrue())))
      .WillOnce([&] { reported = true; });

  metric_service_.RegisterProvider(metric_provider_.AsStdFunction());
  PumpUntil([&] { return reported; });
}

TEST_F(MetricServiceTest, WhenMetricWasCollected_ReportsIt) {
  bool reported = false;
  EXPECT_CALL(metric_provider_, Call())
      .WillOnce([this]() -> Awaitable<Metrics> { co_return MakeMetrics(); });

  EXPECT_CALL(
      metric_sink_,
      Call(Property(&Metrics::ToUnorderedMap,
                    UnorderedElementsAre(Pair(kMetricName, kMetricValue)))))
      .WillOnce([&] { reported = true; });

  metric_service_.RegisterProvider(metric_provider_.AsStdFunction());
  PumpUntil([&] { return reported; });
}

TEST_F(MetricServiceTest, UnregistersProvidersThrowingFromCoroutine) {
  bool attempted = false;
  EXPECT_CALL(metric_provider_, Call())
      .WillOnce([&]() -> Awaitable<Metrics> {
        attempted = true;
        throw std::runtime_error{"stop"};
        co_return Metrics{};
      });

  metric_service_.RegisterProvider(metric_provider_.AsStdFunction());
  PumpUntil([&] { return attempted; });
  asio_env_.PumpFor(kReportPeriod * 2);
}
