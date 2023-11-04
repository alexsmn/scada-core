#include "metrics/metric_service_impl.h"

#include "base/test/test_executor.h"

#include <gmock/gmock.h>

using namespace testing;

class MetricServiceImplTest : public Test {
 public:
  MetricServiceImplTest() {
    metric_service_impl_.RegisterProvider(metric_provider_.AsStdFunction());
  }

 protected:
  Metrics MakeMetrics() {
    Metrics metrics;
    metrics.Set(kMetricName, kMetricValue);
    return metrics;
  }

  const std::shared_ptr<TestExecutor> executor_ =
      std::make_shared<TestExecutor>();

  StrictMock<MockFunction<promise<Metrics>()>> metric_provider_;

  StrictMock<MockFunction<void(const Metrics&)>> metric_reporter_;

  MetricServiceImpl metric_service_impl_{executor_, kReportPeriod,
                                         metric_reporter_.AsStdFunction()};

  inline static const std::chrono::milliseconds kReportPeriod{1000};

  inline static const char kMetricName[] = "Metric";
  inline static const int kMetricValue = 42;
};

TEST_F(MetricServiceImplTest, DoesntTriggerMetricReporterByDefault) {}

TEST_F(MetricServiceImplTest, WhenNoMetricsWereCollected_ReportsEmptyMetrics) {
  EXPECT_CALL(metric_provider_, Call())
      .WillOnce(Return(make_resolved_promise(Metrics{})));
  EXPECT_CALL(metric_reporter_, Call(Property(&Metrics::empty, IsTrue())));

  executor_->Advance(kReportPeriod);
}

TEST_F(MetricServiceImplTest, WhenMetricWasCollected_ReportsIt) {
  EXPECT_CALL(metric_provider_, Call())
      .WillOnce(Return(make_resolved_promise(MakeMetrics())));
  EXPECT_CALL(
      metric_reporter_,
      Call(Property(&Metrics::ToUnorderedMap,
                    UnorderedElementsAre(Pair(kMetricName, kMetricValue)))));

  executor_->Advance(kReportPeriod);
}

TEST_F(MetricServiceImplTest, WhenProviderIsCanceled_DoesntInvokeIt) {
  EXPECT_CALL(metric_provider_, Call())
      .WillOnce(Return(make_rejected_promise<Metrics>(std::exception{})));

  executor_->Advance(kReportPeriod);
  executor_->Advance(kReportPeriod);
}
