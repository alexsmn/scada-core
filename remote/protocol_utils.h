#pragma once

#include "remote/protocol.h"
#include "scada/data_value.h"
#include "scada/monitored_item_service.h"
#include "scada/node_attributes.h"
#include "scada/node_management_service.h"
#include "scada/status.h"
#include "scada/view_service.h"

#include <set>
#include <vector>

namespace scada {
struct AggregateFilter;
struct Event;
struct EventFilter;
struct ModelChangeEvent;
struct WriteValue;
}  // namespace scada

template <class Source, class Target>
void Convert(const Source& source, Target& target);

void Convert(const std::string& source, scada::QualifiedName& target);
void Convert(const scada::QualifiedName& source, std::string& target);

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

void Convert(const protocol::StatusCode& source, scada::StatusCode& target);
void Convert(scada::StatusCode source, protocol::StatusCode& target);

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

void Convert(const protocol::Write& source, scada::WriteValue& target);
void Convert(const scada::WriteValue& source, protocol::Write& target);

void Convert(protocol::BrowseDirection source, scada::BrowseDirection& target);
void Convert(scada::BrowseDirection source, protocol::BrowseDirection& target);

void Convert(const protocol::ReferenceDescription& source,
             scada::ReferenceDescription& target);
void Convert(const scada::ReferenceDescription& source,
             protocol::ReferenceDescription& target);

void Convert(const protocol::BrowseResult& source, scada::BrowseResult& target);
void Convert(const scada::BrowseResult& source, protocol::BrowseResult& target);

void Convert(const protocol::BrowsePath& source, scada::BrowsePath& target);
void Convert(const scada::BrowsePath& source, protocol::BrowsePath& target);

void Convert(const protocol::RelativePathElement& source,
             scada::RelativePathElement& target);
void Convert(const scada::RelativePathElement& source,
             protocol::RelativePathElement& target);

void Convert(const protocol::BrowsePathResult& source,
             scada::BrowsePathResult& target);
void Convert(const scada::BrowsePathResult& source,
             protocol::BrowsePathResult& target);

void Convert(const protocol::BrowsePathTarget& source,
             scada::BrowsePathTarget& target);
void Convert(const scada::BrowsePathTarget& source,
             protocol::BrowsePathTarget& target);

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

void Convert(const protocol::ExpandedNodeId& source,
             scada::ExpandedNodeId& target);
void Convert(const scada::ExpandedNodeId& source,
             protocol::ExpandedNodeId& target);

void Convert(const protocol::AddNode& source, scada::AddNodesItem& target);
void Convert(const scada::AddNodesItem& source, protocol::AddNode& target);

void Convert(const protocol::AddNodeResult& source,
             scada::AddNodesResult& target);
void Convert(const scada::AddNodesResult& source,
             protocol::AddNodeResult& target);

void Convert(const protocol::DeleteNode& source,
             scada::DeleteNodesItem& target);
void Convert(const scada::DeleteNodesItem& source,
             protocol::DeleteNode& target);

void Convert(const protocol::AddReference& source,
             scada::AddReferencesItem& target);
void Convert(const scada::AddReferencesItem& source,
             protocol::AddReference& target);

void Convert(const protocol::DeleteReference& source,
             scada::DeleteReferencesItem& target);
void Convert(const scada::DeleteReferencesItem& source,
             protocol::DeleteReference& target);

// Generalization.
template <class T>
inline void Convert(const T& source, T& target) {
  target = source;
}

// Convert To/From Repeated

template <class Target, class Source>
Target ConvertTo(const Source& source);

template <typename Target, typename SourceRepeatedField>
inline void ConvertFromRepeated(const SourceRepeatedField& source,
                                std::vector<Target>& target) {
  target.reserve(target.size() + source.size());
  for (auto& s : source)
    Convert(s, target.emplace_back());
}

// It seems std::vector<bool>::emplace_back() doesn't return a writable
// reference.
template <typename SourceRepeatedField>
inline void ConvertFromRepeated(const SourceRepeatedField& source,
                                std::vector<bool>& target) {
  target.reserve(target.size() + source.size());
  for (auto& s : source)
    target.emplace_back(ConvertTo<bool>(s));
}

template <typename Target, typename SourceRepeatedField>
inline void ConvertFromRepeated(const SourceRepeatedField& source,
                                std::set<Target>& target) {
  target.reserve(target.size() + source.size());
  for (auto& s : source)
    target.emplace(Convert<Target>(source));
}

template <typename TargetRepeatedField, typename Container>
inline void ConvertToRepeated(const Container& source,
                              TargetRepeatedField& target) {
  target.Reserve(target.size() + source.size());
  for (auto&& s : source)
    Convert(s, *target.Add());
}

// vector <-> RepeatedField

template <typename Source, typename Target>
inline void Convert(const std::vector<Source>& sources,
                    ::google::protobuf::RepeatedField<Target>& targets) {
  ConvertToRepeated(sources, targets);
}

template <typename Source, typename Target>
inline void Convert(const ::google::protobuf::RepeatedField<Target>& sources,
                    std::vector<Source>& targets) {
  ConvertFromRepeated(sources, targets);
}

// set <-> RepeatedField

template <typename Source, typename Target>
inline void Convert(const std::set<Source>& sources,
                    ::google::protobuf::RepeatedField<Target>& targets) {
  ConvertToRepeated(sources, targets);
}

template <typename Source, typename Target>
inline void Convert(const ::google::protobuf::RepeatedField<Target>& sources,
                    std::set<Source>& targets) {
  ConvertFromRepeated(sources, targets);
}

// vector <-> RepeatedPtrField

template <typename Source, typename Target>
inline void Convert(const std::vector<Source>& sources,
                    ::google::protobuf::RepeatedPtrField<Target>& targets) {
  ConvertToRepeated(sources, targets);
}

template <typename Source, typename Target>
inline void Convert(const ::google::protobuf::RepeatedPtrField<Target>& sources,
                    std::vector<Source>& targets) {
  ConvertFromRepeated(sources, targets);
}

// set <-> RepeatedPtrField

template <typename Source, typename Target>
inline void Convert(const std::set<Source>& sources,
                    ::google::protobuf::RepeatedPtrField<Target>& targets) {
  ConvertToRepeated(sources, targets);
}

template <typename Source, typename Target>
inline void Convert(const ::google::protobuf::RepeatedPtrField<Target>& sources,
                    std::set<Source>& targets) {
  ConvertFromRepeated(sources, targets);
}

// ConvertTo

template <class Target, class Source>
inline Target ConvertTo(const Source& source) {
  Target target{};
  Convert(source, target);
  return target;
}

template <>
inline scada::Status ConvertTo(const protocol::Status& source) {
  scada::Status target{scada::StatusCode::Bad};
  Convert(source, target);
  return target;
}

template <>
inline std::vector<scada::Status> ConvertTo(
    const ::google::protobuf::RepeatedPtrField<protocol::Status>& source) {
  std::vector<scada::Status> target(source.size(), scada::StatusCode::Bad);
  // For some reason |RepeatedPtrField::size()| is int.
  for (size_t i = 0; i < static_cast<size_t>(source.size()); ++i)
    Convert(source[i], target[i]);
  return target;
}
