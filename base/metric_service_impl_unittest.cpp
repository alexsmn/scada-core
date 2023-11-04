#include "base/metric_service_impl.h"

#include "base/test/test_executor.h"

#include <gmock/gmock.h>

using namespace testing;

class MetricServiceImplTest : public Test {
 public:
  MetricServiceImplTest() {
    metric_service_impl_.RegisterProvider(metric_provider_.AsStdFunction(),
                                          cancelation_.ref());
  }

 protected:
  const std::shared_ptr<TestExecutor> executor_ =
      std::make_shared<TestExecutor>();

  Cancelation cancelation_;

  StrictMock<MockFunction<void(MetricService::Metrics&)>> metric_provider_;

  StrictMock<MockFunction<void(const MetricService::Metrics&)>>
      metric_reporter_;

  MetricServiceImpl metric_service_impl_{executor_, kReportPeriod,
                                         metric_reporter_.AsStdFunction()};

  inline static const std::chrono::milliseconds kReportPeriod{1000};

  inline static const char kMetricName[] = "Metric";
  inline static const int kMetricValue = 42;
};

TEST_F(MetricServiceImplTest, DoesntTriggerMetricReporterByDefault) {}

TEST_F(MetricServiceImplTest, WhenNoMetricsWereCollected_ReportsEmptyMetrics) {
  EXPECT_CALL(metric_provider_, Call(_))
      .WillOnce(Invoke([](MetricService::Metrics& metrics) {}));

  EXPECT_CALL(metric_reporter_,
              Call(Property(&MetricService::Metrics::empty, IsTrue())));

  executor_->Advance(kReportPeriod);
}

TEST_F(MetricServiceImplTest, WhenMetricWasCollected_ReportsIt) {
  EXPECT_CALL(metric_provider_, Call(_))
      .WillOnce(Invoke([](MetricService::Metrics& metrics) {
        metrics.Set(kMetricName, kMetricValue);
      }));

  EXPECT_CALL(
      metric_reporter_,
      Call(Property(&MetricService::Metrics::ToUnorderedMap,
                    UnorderedElementsAre(Pair(kMetricName, kMetricValue)))));

  executor_->Advance(kReportPeriod);
}

TEST_F(MetricServiceImplTest, WhenProviderIsCanceled_DoesntInvokeIt) {
  cancelation_.Cancel();

  EXPECT_CALL(metric_reporter_,
              Call(Property(&MetricService::Metrics::empty, IsTrue())));

  executor_->Advance(kReportPeriod);
}
