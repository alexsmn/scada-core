#pragma once

#include "base/cancelation.h"

#include <functional>
#include <string>
#include <unordered_map>

class MetricService {
 public:
  virtual ~MetricService() = default;

  using MetricValue = int;

  class Metrics {
   public:
    void Set(const std::string& name, const MetricValue& value) {
      values_.insert_or_assign(name, value);
    }

    // visitor: (const std::string& name,
    //           const MetricService::MetricValue& value)
    template <class T>
    void Visit(T&& visitor) const {
      for (const auto& [name, value] : values_) {
        visitor(name, value);
      }
    }

    bool empty() const { return values_.empty(); }

    std::unordered_map<std::string, MetricValue> ToUnorderedMap() const {
      return values_;
    }

   private:
    std::unordered_map<std::string, MetricValue> values_;
  };

  using Provider = std::function<void(Metrics& metrics)>;

  virtual void RegisterProvider(const Provider& provider,
                                const CancelationRef& cancelation) = 0;
};