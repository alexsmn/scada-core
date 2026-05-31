#include "metrics/metric_service_impl.h"

#include "base/awaitable.h"
#include "base/test/asio_test_environment.h"
#include "scada/status_or.h"

#include <gmock/gmock.h>

using namespace testing;

using MetricProviderResult = Awaitable<scada::StatusOr<Metrics>>;

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

  StrictMock<MockFunction<MetricProviderResult()>> metric_provider_;
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
      .WillOnce([]() -> MetricProviderResult {
        co_return Metrics{};
      });

  EXPECT_CALL(metric_sink_, Call(Property(&Metrics::empty, IsTrue())))
      .WillOnce([&] { reported = true; });

  metric_service_.RegisterProvider(metric_provider_.AsStdFunction());
  PumpUntil([&] { return reported; });
}

TEST_F(MetricServiceTest, WhenMetricWasCollected_ReportsIt) {
  bool reported = false;
  EXPECT_CALL(metric_provider_, Call())
      .WillOnce([this]() -> MetricProviderResult {
        co_return MakeMetrics();
      });

  EXPECT_CALL(
      metric_sink_,
      Call(Property(&Metrics::ToUnorderedMap,
                    UnorderedElementsAre(Pair(kMetricName, kMetricValue)))))
      .WillOnce([&] { reported = true; });

  metric_service_.RegisterProvider(metric_provider_.AsStdFunction());
  PumpUntil([&] { return reported; });
}

TEST_F(MetricServiceTest, UnregistersProvidersReturningBadStatus) {
  bool attempted = false;
  EXPECT_CALL(metric_provider_, Call())
      .WillOnce([&]() -> MetricProviderResult {
        attempted = true;
        co_return scada::StatusCode::Bad;
      });

  metric_service_.RegisterProvider(metric_provider_.AsStdFunction());
  PumpUntil([&] { return attempted; });
  asio_env_.PumpFor(kReportPeriod * 2);
}
