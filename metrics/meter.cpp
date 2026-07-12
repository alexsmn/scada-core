#include "metrics/meter.h"

#include <opentelemetry/common/key_value_iterable_view.h>
#include <opentelemetry/metrics/provider.h>

#include <mutex>
#include <unordered_map>
#include <vector>

#if defined(SCADA_USE_BASE_MODULE)
// Modules-pilot consumer (SCADA_CXX_MODULES=ON): base names come from the
// scada.base facade. The import sits after the textual includes because the
// reverse order trips an AppleClang 21 declaration-merging bug in libc++.
import scada.base;
#else
#include "base/check.h"
#include "base/map_util.h"
#endif

namespace scada::metrics {

namespace {

namespace metrics_api = opentelemetry::metrics;

template <class Instrument>
Instrument& FindOrCreateInstrument(
    std::unordered_map<std::string, std::unique_ptr<Instrument>>& instruments,
    std::mutex& mutex,
    std::string_view metric_name,
    auto create) {
  std::lock_guard lock{mutex};

  const std::string name{metric_name};
  if (auto* instrument = FindPtr(instruments, name)) {
    return **instrument;
  }

  auto [iter, inserted] = instruments.try_emplace(name);
  base::Check(inserted);
  iter->second = create(name);

  return *iter->second;
}

std::vector<std::pair<opentelemetry::nostd::string_view,
                      opentelemetry::common::AttributeValue>>
MakeOtelAttributes(const MetricAttributes& attributes) {
  std::vector<std::pair<opentelemetry::nostd::string_view,
                        opentelemetry::common::AttributeValue>>
      result;
  result.reserve(attributes.size());
  for (const auto& [name, value] : attributes) {
    result.emplace_back(opentelemetry::nostd::string_view{name},
                        opentelemetry::nostd::string_view{value});
  }
  return result;
}

MetricAttributes MergeAttributes(const MetricAttributes& meter_attributes,
                                 const MetricAttributes& attributes) {
  MetricAttributes result = meter_attributes;
  for (const auto& [name, value] : attributes) {
    result.insert_or_assign(name, value);
  }
  return result;
}

opentelemetry::nostd::span<
    const std::pair<opentelemetry::nostd::string_view,
                    opentelemetry::common::AttributeValue>>
MakeOtelAttributeSpan(
    const std::vector<std::pair<opentelemetry::nostd::string_view,
                                opentelemetry::common::AttributeValue>>&
        attributes) {
  return {attributes.data(), attributes.size()};
}

}  // namespace

class Meter::Impl {
 public:
  Impl(std::string meter_name, MetricAttributes attributes)
      : meter_{metrics_api::Provider::GetMeterProvider()->GetMeter(meter_name,
                                                                   "1.0.0")},
        attributes_{std::move(attributes)} {}

  void AddCounter(std::string_view metric_name,
                  std::uint64_t value,
                  const MetricAttributes& attributes) {
    auto& counter = FindOrCreateInstrument(
        counters_, mutex_, metric_name, [this](std::string_view name) {
          return meter_->CreateUInt64Counter(std::string{name});
        });
    const auto merged_attributes = MergeAttributes(attributes_, attributes);
    const auto otel_attributes = MakeOtelAttributes(merged_attributes);
    counter.Add(value, MakeOtelAttributeSpan(otel_attributes));
  }

  void AddUpDownCounter(std::string_view metric_name,
                        std::int64_t delta,
                        const MetricAttributes& attributes) {
    auto& counter = FindOrCreateInstrument(
        up_down_counters_, mutex_, metric_name, [this](std::string_view name) {
          return meter_->CreateInt64UpDownCounter(std::string{name});
        });
    const auto merged_attributes = MergeAttributes(attributes_, attributes);
    const auto otel_attributes = MakeOtelAttributes(merged_attributes);
    counter.Add(delta, MakeOtelAttributeSpan(otel_attributes));
  }

  void RecordHistogram(std::string_view metric_name,
                       double value,
                       const MetricAttributes& attributes) {
    auto& histogram = FindOrCreateInstrument(
        histograms_, mutex_, metric_name, [this](std::string_view name) {
          return meter_->CreateDoubleHistogram(std::string{name});
        });
    const auto merged_attributes = MergeAttributes(attributes_, attributes);
    const auto otel_attributes = MakeOtelAttributes(merged_attributes);
    histogram.Record(value, MakeOtelAttributeSpan(otel_attributes),
                     opentelemetry::context::Context{});
  }

 private:
  opentelemetry::nostd::shared_ptr<metrics_api::Meter> meter_;
  MetricAttributes attributes_;
  std::mutex mutex_;
  std::unordered_map<std::string,
                     std::unique_ptr<metrics_api::Counter<std::uint64_t>>>
      counters_;
  std::unordered_map<std::string,
                     std::unique_ptr<metrics_api::UpDownCounter<std::int64_t>>>
      up_down_counters_;
  std::unordered_map<std::string,
                     std::unique_ptr<metrics_api::Histogram<double>>>
      histograms_;
};

Meter::Meter(std::string meter_name) : Meter{std::move(meter_name), {}} {}

Meter::Meter(std::string meter_name, MetricAttributes attributes)
    : impl_{std::make_unique<Impl>(std::move(meter_name),
                                   std::move(attributes))} {}

Meter::~Meter() = default;

void Meter::AddCounter(std::string_view metric_name,
                       std::uint64_t value,
                       const MetricAttributes& attributes) {
  impl_->AddCounter(metric_name, value, attributes);
}

void Meter::AddUpDownCounter(std::string_view metric_name,
                             std::int64_t delta,
                             const MetricAttributes& attributes) {
  impl_->AddUpDownCounter(metric_name, delta, attributes);
}

void Meter::RecordHistogram(std::string_view metric_name,
                            double value,
                            const MetricAttributes& attributes) {
  impl_->RecordHistogram(metric_name, value, attributes);
}

}  // namespace scada::metrics
