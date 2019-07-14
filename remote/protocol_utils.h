#pragma once

#include <set>
#include <vector>

#include "core/aggregate_filter.h"
#include "core/configuration_types.h"
#include "core/data_value.h"
#include "core/event.h"
#include "core/monitored_item_service.h"
#include "core/node_attributes.h"
#include "core/status.h"
#include "core/view_service.h"
#include "remote/protocol.h"

void Convert(const std::string& source, scada::LocalizedText& target);
void Convert(const scada::LocalizedText& source, std::string& target);

void Convert(const std::string& source, scada::ByteString& target);
void Convert(const scada::ByteString& source, std::string& target);

void Convert(const protocol::NodeId& source, scada::NodeId& target);
void Convert(const scada::NodeId& source, protocol::NodeId& target);

void Convert(const protocol::Variant& source, scada::Variant& target);
void Convert(const scada::Variant& source, protocol::Variant& target);

void Convert(const protocol::DataValue& source, scada::DataValue& target);
void Convert(const scada::DataValue& source, protocol::DataValue& target);

void Convert(const protocol::Status& source, scada::Status& target);
void Convert(const scada::Status& source, protocol::Status& target);

void Convert(const protocol::Event& source, scada::Event& target);
void Convert(const scada::Event& source, protocol::Event& target);

void Convert(const protocol::EventFilter& source, scada::EventFilter& target);
void Convert(const scada::EventFilter& source, protocol::EventFilter& target);

void Convert(const protocol::NodeClass source, scada::NodeClass& target);
void Convert(const scada::NodeClass source, protocol::NodeClass& target);

void Convert(const protocol::Attributes& source, scada::NodeAttributes& target);
void Convert(const scada::NodeAttributes& source, protocol::Attributes& target);

void Convert(protocol::AttributeId source, scada::AttributeId& target);
void Convert(scada::AttributeId source, protocol::AttributeId& target);

void Convert(const protocol::ReadValueId& source, scada::ReadValueId& target);
void Convert(const scada::ReadValueId& source, protocol::ReadValueId& target);

void Convert(protocol::BrowseDirection source, scada::BrowseDirection& target);
void Convert(scada::BrowseDirection source, protocol::BrowseDirection& target);

void Convert(const protocol::ReferenceDescription& source,
             scada::ReferenceDescription& target);
void Convert(const scada::ReferenceDescription& source,
             protocol::ReferenceDescription& target);

void Convert(const protocol::BrowseResult& source, scada::BrowseResult& target);
void Convert(const scada::BrowseResult& source, protocol::BrowseResult& target);

void Convert(const protocol::ModelChangeEvent& source,
             scada::ModelChangeEvent& target);
void Convert(const scada::ModelChangeEvent& source,
             protocol::ModelChangeEvent& target);

void Convert(const protocol::AggregateFilter& source,
             scada::AggregateFilter& target);
void Convert(const scada::AggregateFilter& source,
             protocol::AggregateFilter& target);

void Convert(const protocol::MonitoringParameters& source,
             scada::MonitoringParameters& target);
void Convert(const scada::MonitoringParameters& source,
             protocol::MonitoringParameters& target);

template <typename Target, typename Source>
inline void Convert(const ::google::protobuf::RepeatedPtrField<Source>& source,
                    std::vector<Target>& target) {
  target.reserve(target.size() + source.size());
  for (auto& s : source)
    Convert(s, target.emplace_back());
}

template <typename Target, typename Container>
inline void ConvertContainer(
    const Container& source,
    ::google::protobuf::RepeatedPtrField<Target>& target) {
  target.Reserve(target.size() + source.size());
  for (auto& s : source)
    Convert(s, *target.Add());
}

template <typename Source, typename Target>
inline void Convert(const std::vector<Source>& sources,
                    ::google::protobuf::RepeatedPtrField<Target>& targets) {
  ConvertContainer(sources, targets);
}

template <typename Source, typename Target>
inline void Convert(const std::set<Source>& sources,
                    ::google::protobuf::RepeatedPtrField<Target>& targets) {
  ConvertContainer(sources, targets);
}

template <typename Target, typename Source>
inline void Convert(const ::google::protobuf::RepeatedPtrField<Source>& source,
                    std::set<Target>& target) {
  for (auto& s : source)
    Convert(s, target.emplace());
}

template <class Target, class Source>
inline Target Convert(const Source& source) {
  Target target{};
  Convert(source, target);
  return target;
}

template <>
inline scada::Status Convert(const protocol::Status& source) {
  scada::Status target{scada::StatusCode::Bad};
  Convert(source, target);
  return target;
}

template <>
inline std::vector<scada::Status> Convert(
    const ::google::protobuf::RepeatedPtrField<protocol::Status>& source) {
  std::vector<scada::Status> target(source.size(), scada::StatusCode::Bad);
  for (size_t i = 0; i < source.size(); ++i)
    Convert(source[i], target[i]);
  return target;
}
