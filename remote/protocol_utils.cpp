#include "remote/protocol_utils.h"

#include "base/strings/utf_string_conversions.h"
#include "core/standard_node_ids.h"

namespace {

template <class Source, class Target>
inline void CastConvert(const std::vector<Source>& source,
                        ::google::protobuf::RepeatedField<Target>& target) {
  target.Reserve(target.size() + source.size());
  for (auto& s : source)
    target.Add(static_cast<Target>(s));
}

template <class Source, class Target>
inline void CastConvert(const ::google::protobuf::RepeatedField<Target>& source,
                        std::vector<Source>& target) {
  target.insert(target.end(), source.begin(), source.end());
}

template <class Target, class Source>
inline Target CastConvertTo(const Source& source) {
  Target target;
  CastConvert(source, target);
  return target;
}

}  // namespace

void Convert(const protocol::NodeId& source, scada::NodeId& target) {
  auto namespace_index =
      static_cast<scada::NamespaceIndex>(source.namespace_index());
  if (!source.string_id().empty())
    target = scada::NodeId(source.string_id(), namespace_index);
  else if (!source.opaque_id().empty())
    target = scada::NodeId(source.opaque_id(), namespace_index);
  else
    target = scada::NodeId(source.numeric_id(), namespace_index);
}

void Convert(const scada::NodeId& source, protocol::NodeId& target) {
  if (source.namespace_index() != 0)
    target.set_namespace_index(source.namespace_index());

  switch (source.type()) {
    case scada::NodeIdType::Numeric:
      if (source.numeric_id() != 0)
        target.set_numeric_id(source.numeric_id());
      break;
    case scada::NodeIdType::String:
      target.set_string_id(source.string_id());
      break;
    case scada::NodeIdType::Opaque:
      target.set_opaque_id(source.opaque_id().data(),
                           source.opaque_id().size());
      break;
    default:
      assert(false);
      break;
  }
}

void Convert(const std::string& source, scada::LocalizedText& target) {
  target = base::UTF8ToUTF16(source);
}

void Convert(const scada::LocalizedText& source, std::string& target) {
  target = base::UTF16ToUTF8(ToString16(source));
}

void Convert(const scada::String& source, std::string& target) {
  target = source;
}

void Convert(const scada::ByteString& source, std::string& target) {
  target.assign(source.begin(), source.end());
}

void Convert(const std::string& source, scada::ByteString& target) {
  target.assign(source.begin(), source.end());
}

void Convert(const std::string& source, scada::QualifiedName& target) {
  target = source;
}

void Convert(const scada::QualifiedName& source, std::string& target) {
  target = source.name();
}

static_assert(static_cast<size_t>(scada::Variant::COUNT) == 19);

void Convert(const protocol::Variant& source, scada::Variant& target) {
  if (source.rank() == 0) {
    switch (static_cast<scada::Variant::Type>(source.data_type())) {
      case scada::Variant::EMPTY:
        target = {};
        break;
      case scada::Variant::BOOL:
        target = source.bool_value();
        break;
      case scada::Variant::INT8:
        target = static_cast<scada::Int8>(source.int_value());
        break;
      case scada::Variant::UINT8:
        target = static_cast<scada::UInt8>(source.int_value());
        break;
      case scada::Variant::INT16:
        target = static_cast<scada::Int16>(source.int_value());
        break;
      case scada::Variant::UINT16:
        target = static_cast<scada::UInt16>(source.int_value());
        break;
      case scada::Variant::INT32:
        target = static_cast<scada::Int32>(source.int_value());
        break;
      case scada::Variant::UINT32:
        target = static_cast<scada::UInt32>(source.int_value());
        break;
      case scada::Variant::INT64:
        target = static_cast<scada::Int64>(source.int_value());
        break;
      case scada::Variant::UINT64:
        target = static_cast<scada::UInt64>(source.int_value());
        break;
      case scada::Variant::DOUBLE:
        target = source.double_value();
        break;
      case scada::Variant::STRING:
        target = scada::String{source.string_value_utf8()};
        break;
      case scada::Variant::QUALIFIED_NAME:
        target = ConvertTo<scada::QualifiedName>(source.string_value_utf8());
        break;
      case scada::Variant::LOCALIZED_TEXT:
        target = ConvertTo<scada::LocalizedText>(source.string_value_utf8());
        break;
      case scada::Variant::BYTE_STRING:
        target = ConvertTo<scada::ByteString>(source.byte_string_value());
        break;
      case scada::Variant::NODE_ID:
        target = ConvertTo<scada::NodeId>(source.node_id_value());
        break;
      case scada::Variant::DATE_TIME:
        target = scada::DateTime::FromDeltaSinceWindowsEpoch(
            base::TimeDelta::FromMicroseconds(source.time_value_time()));
        break;
      default:
        assert(false);
        target = {};
        break;
    }

  } else if (source.rank() == 1) {
    switch (static_cast<scada::Variant::Type>(source.data_type())) {
      case scada::Variant::BOOL:
        target = ConvertTo<std::vector<scada::Boolean>>(source.bool_array());
        break;
      case scada::Variant::INT8:
        target = CastConvertTo<std::vector<scada::Int8>>(source.int_array());
        break;
      case scada::Variant::UINT8:
        target = CastConvertTo<std::vector<scada::UInt8>>(source.int_array());
        break;
      case scada::Variant::INT16:
        target = CastConvertTo<std::vector<scada::Int16>>(source.int_array());
        break;
      case scada::Variant::UINT16:
        target = CastConvertTo<std::vector<scada::UInt16>>(source.int_array());
        break;
      case scada::Variant::INT32:
        target = CastConvertTo<std::vector<scada::Int32>>(source.int_array());
        break;
      case scada::Variant::UINT32:
        target = CastConvertTo<std::vector<scada::UInt32>>(source.int_array());
        break;
      case scada::Variant::INT64:
        target = CastConvertTo<std::vector<scada::Int64>>(source.int_array());
        break;
      case scada::Variant::UINT64:
        target = CastConvertTo<std::vector<scada::UInt64>>(source.int_array());
        break;
      case scada::Variant::DOUBLE:
        target = ConvertTo<std::vector<scada::Double>>(source.double_array());
        break;
      case scada::Variant::STRING:
        target =
            ConvertTo<std::vector<scada::String>>(source.string_array_utf8());
        break;
      case scada::Variant::LOCALIZED_TEXT:
        target = ConvertTo<std::vector<scada::LocalizedText>>(
            source.string_array_utf8());
        break;
      case scada::Variant::EXTENSION_OBJECT:
        // Ignore.
        target = {};
        break;
      default:
        assert(false);
        target = {};
        break;
    }

  } else {
    assert(false);
    target = {};
  }
}

static_assert(static_cast<size_t>(scada::Variant::COUNT) == 19);

void Convert(const scada::Variant& source, protocol::Variant& target) {
  if (source.is_null())
    return;

  target.set_data_type(static_cast<int>(source.type()));

  if (source.is_scalar()) {
    switch (source.type()) {
      case scada::Variant::EMPTY:
        break;
      case scada::Variant::BOOL:
        target.set_bool_value(source.as_bool());
        break;
      case scada::Variant::INT8:
        target.set_int_value(source.get<scada::Int8>());
        break;
      case scada::Variant::UINT8:
        target.set_int_value(source.get<scada::UInt8>());
        break;
      case scada::Variant::INT16:
        target.set_int_value(source.get<scada::Int16>());
        break;
      case scada::Variant::UINT16:
        target.set_int_value(source.get<scada::UInt16>());
        break;
      case scada::Variant::INT32:
        target.set_int_value(source.get<scada::Int32>());
        break;
      case scada::Variant::UINT32:
        target.set_int_value(source.get<scada::UInt32>());
        break;
      case scada::Variant::INT64:
        target.set_int_value(source.get<scada::Int64>());
        break;
      case scada::Variant::UINT64:
        target.set_int_value(source.get<scada::UInt64>());
        break;
      case scada::Variant::DOUBLE:
        target.set_double_value(source.get<scada::Double>());
        break;
      case scada::Variant::STRING:
        target.set_string_value_utf8(source.as_string());
        break;
      case scada::Variant::QUALIFIED_NAME:
        target.set_string_value_utf8(source.get<scada::QualifiedName>().name());
        break;
      case scada::Variant::LOCALIZED_TEXT:
        Convert(source.as_localized_text(),
                *target.mutable_string_value_utf8());
        break;
      case scada::Variant::BYTE_STRING:
        Convert(source.get<scada::ByteString>(),
                *target.mutable_byte_string_value());
        break;
      case scada::Variant::NODE_ID:
        Convert(source.as_node_id(), *target.mutable_node_id_value());
        break;
      case scada::Variant::DATE_TIME:
        target.set_time_value_time(source.get<scada::DateTime>()
                                       .ToDeltaSinceWindowsEpoch()
                                       .InMicroseconds());
        break;
      default:
        assert(false);
        break;
    }

  } else {
    target.set_rank(1);
    switch (source.type()) {
      case scada::Variant::BOOL:
        Convert(source.get<std::vector<scada::Boolean>>(),
                *target.mutable_bool_array());
        break;
      case scada::Variant::INT8:
        CastConvert(source.get<std::vector<scada::Int8>>(),
                    *target.mutable_int_array());
        break;
      case scada::Variant::UINT8:
        CastConvert(source.get<std::vector<scada::UInt8>>(),
                    *target.mutable_int_array());
        break;
      case scada::Variant::INT16:
        CastConvert(source.get<std::vector<scada::Int16>>(),
                    *target.mutable_int_array());
        break;
      case scada::Variant::UINT16:
        CastConvert(source.get<std::vector<scada::UInt16>>(),
                    *target.mutable_int_array());
        break;
      case scada::Variant::INT32:
        CastConvert(source.get<std::vector<scada::Int32>>(),
                    *target.mutable_int_array());
        break;
      case scada::Variant::UINT32:
        CastConvert(source.get<std::vector<scada::UInt32>>(),
                    *target.mutable_int_array());
        break;
      case scada::Variant::INT64:
        CastConvert(source.get<std::vector<scada::Int64>>(),
                    *target.mutable_int_array());
        break;
      case scada::Variant::UINT64:
        CastConvert(source.get<std::vector<scada::UInt64>>(),
                    *target.mutable_int_array());
        break;
      case scada::Variant::DOUBLE:
        Convert(source.get<std::vector<scada::Double>>(),
                *target.mutable_double_array());
        break;
      case scada::Variant::STRING:
        Convert(source.get<std::vector<scada::String>>(),
                *target.mutable_string_array_utf8());
        break;
      case scada::Variant::LOCALIZED_TEXT:
        Convert(source.get<std::vector<scada::LocalizedText>>(),
                *target.mutable_string_array_utf8());
        break;
      case scada::Variant::EXTENSION_OBJECT:
        // Ignore.
        break;
      default:
        assert(false);
        break;
    }
  }
}

void Convert(const protocol::DataValue& source, scada::DataValue& target) {
  if (source.server_time())
    target.server_timestamp =
        base::Time::FromInternalValue(source.server_time());
  if (source.source_time())
    target.source_timestamp =
        base::Time::FromInternalValue(source.source_time());
  if (source.has_value())
    Convert(source.value(), target.value);
  target.qualifier = scada::Qualifier(source.qualifier());
  target.status_code = static_cast<scada::StatusCode>(source.status_code());
}

void Convert(const scada::DataValue& source, protocol::DataValue& target) {
  target.set_server_time(source.server_timestamp.ToInternalValue());
  target.set_source_time(source.source_timestamp.ToInternalValue());
  if (!source.value.is_null())
    Convert(source.value, *target.mutable_value());
  target.set_qualifier(source.qualifier.raw());
  target.set_status_code(
      static_cast<google::protobuf::uint32>(source.status_code));
}

void Convert(const protocol::StatusCode& source, scada::StatusCode& target) {
  target = scada::MakeStatusCode(
      static_cast<scada::StatusSeverity>(source.severity()), source.sub_code());
}

void Convert(scada::StatusCode source, protocol::StatusCode& target) {
  target.set_severity(
      static_cast<protocol::Severity>(scada::GetSeverity(source)));
  target.set_sub_code(scada::GetSubCode(source));
}

void Convert(const protocol::Status& source, scada::Status& target) {
  target = ConvertTo<scada::StatusCode>(source.code());
}

void Convert(const scada::Status& source, protocol::Status& target) {
  Convert(source.code(), *target.mutable_code());
}

void Convert(const protocol::Event& source, scada::Event& target) {
  target.time = base::Time::FromInternalValue(source.time());
  target.severity = source.severity();
  if (source.has_source_node_id())
    Convert(source.source_node_id(), target.node_id);
  if (source.has_user_node_id())
    Convert(source.user_node_id(), target.user_id);
  if (source.has_value())
    Convert(source.value(), target.value);
  target.qualifier = scada::Qualifier(source.qualifier());
  target.message = base::UTF8ToUTF16(source.message_utf8());
  target.acked = source.acknowledged();
  target.acknowledge_id = source.acknowledge_id();
  if (source.acknowledge_time())
    target.acknowledged_time =
        base::Time::FromInternalValue(source.acknowledge_time());
  if (source.has_acknowledge_user_id())
    Convert(source.acknowledge_user_id(), target.acknowledged_user_id);
}

void Convert(const scada::Event& source, protocol::Event& target) {
  target.set_time(source.time.ToInternalValue());
  target.set_severity(source.severity);
  if (!source.node_id.is_null())
    Convert(source.node_id, *target.mutable_source_node_id());
  if (!source.user_id.is_null())
    Convert(source.user_id, *target.mutable_user_node_id());
  if (!source.value.is_null())
    Convert(source.value, *target.mutable_value());
  if (source.qualifier != scada::Qualifier())
    target.set_qualifier(source.qualifier.raw());
  if (!source.message.empty())
    target.set_message_utf8(base::UTF16ToUTF8(source.message));
  if (source.acked)
    target.set_acknowledged(true);
  if (source.acknowledge_id)
    target.set_acknowledge_id(source.acknowledge_id);
  if (!source.acknowledged_time.is_null())
    target.set_acknowledge_time(source.acknowledged_time.ToInternalValue());
  if (!source.acknowledged_user_id.is_null())
    Convert(source.acknowledged_user_id, *target.mutable_acknowledge_user_id());
  target.set_acknowledge_id(source.acknowledge_id);
}

void Convert(const protocol::EventFilter& source, scada::EventFilter& target) {
  target.types = 0;
  if (source.acked())
    target.types |= scada::EventFilter::ACKED;
  if (source.unacked())
    target.types |= scada::EventFilter::UNACKED;

  Convert(source.of_type(), target.of_type);
  Convert(source.child_of(), target.child_of);
}

void Convert(const scada::EventFilter& source, protocol::EventFilter& target) {
  if (source.types & scada::EventFilter::ACKED)
    target.set_acked(true);
  if (source.types & scada::EventFilter::UNACKED)
    target.set_unacked(true);

  Convert(source.of_type, *target.mutable_of_type());
  Convert(source.child_of, *target.mutable_child_of());
}

void Convert(const protocol::NodeClass source, scada::NodeClass& target) {
  target = static_cast<scada::NodeClass>(static_cast<int>(source));
}

void Convert(const scada::NodeClass source, protocol::NodeClass& target) {
  target = static_cast<protocol::NodeClass>(static_cast<int>(source));
}

void Convert(const protocol::Attributes& source,
             scada::NodeAttributes& target) {
  Convert(source.browse_name_utf8(), target.browse_name);
  Convert(source.display_name_utf8(), target.display_name);
  if (source.has_data_type_id())
    Convert(source.data_type_id(), target.data_type);
  if (source.has_value())
    Convert(source.value(), target.value.emplace());
}

void Convert(const scada::NodeAttributes& source,
             protocol::Attributes& target) {
  target.set_browse_name_utf8(source.browse_name.name());
  target.set_display_name_utf8(
      base::UTF16ToUTF8(ToString16(source.display_name)));
  if (!source.data_type.is_null())
    Convert(source.data_type, *target.mutable_data_type_id());
  if (source.value.has_value())
    Convert(*source.value, *target.mutable_value());
}

void Convert(protocol::AttributeId source, scada::AttributeId& target) {
  target = static_cast<scada::AttributeId>(source);
}

void Convert(scada::AttributeId source, protocol::AttributeId& target) {
  target = static_cast<protocol::AttributeId>(source);
}

void Convert(const protocol::ReadValueId& source, scada::ReadValueId& target) {
  Convert(source.node_id(), target.node_id);
  Convert(source.attribute_id(), target.attribute_id);
}

void Convert(const scada::ReadValueId& source, protocol::ReadValueId& target) {
  Convert(source.node_id, *target.mutable_node_id());
  target.set_attribute_id(
      ConvertTo<protocol::AttributeId>(source.attribute_id));
}

void Convert(const protocol::Write& source, scada::WriteValue& target) {
  Convert(source.node_id(), target.node_id);
  Convert(source.attribute_id(), target.attribute_id);
  Convert(source.value(), target.value);
  if (source.select())
    target.flags.set_select();
}

void Convert(const scada::WriteValue& source, protocol::Write& target) {
  Convert(source.node_id, *target.mutable_node_id());
  target.set_attribute_id(
      ConvertTo<protocol::AttributeId>(source.attribute_id));
  Convert(source.value, *target.mutable_value());
  if (source.flags.select())
    target.set_select(true);
}

void Convert(protocol::BrowseDirection source, scada::BrowseDirection& target) {
  target = static_cast<scada::BrowseDirection>(source);
}

void Convert(scada::BrowseDirection source, protocol::BrowseDirection& target) {
  target = static_cast<protocol::BrowseDirection>(source);
}

void Convert(const protocol::ReferenceDescription& source,
             scada::ReferenceDescription& target) {
  Convert(source.reference_type_id(), target.reference_type_id);
  target.forward = source.forward();
  Convert(source.node_id(), target.node_id);
}

void Convert(const scada::ReferenceDescription& source,
             protocol::ReferenceDescription& target) {
  Convert(source.reference_type_id, *target.mutable_reference_type_id());
  target.set_forward(source.forward);
  Convert(source.node_id, *target.mutable_node_id());
}

void Convert(const protocol::BrowseResult& source,
             scada::BrowseResult& target) {
  target.status_code = static_cast<scada::StatusCode>(source.status_code());
  Convert(source.references(), target.references);
}

void Convert(const scada::BrowseResult& source,
             protocol::BrowseResult& target) {
  target.set_status_code(static_cast<uint32_t>(source.status_code));
  Convert(source.references, *target.mutable_references());
}

void Convert(const protocol::BrowsePath& source, scada::BrowsePath& target) {
  Convert(source.node_id(), target.node_id);
  Convert(source.relative_path_element(), target.relative_path);
}

void Convert(const scada::BrowsePath& source, protocol::BrowsePath& target) {
  Convert(source.node_id, *target.mutable_node_id());
  Convert(source.relative_path, *target.mutable_relative_path_element());
}

void Convert(const protocol::RelativePathElement& source,
             scada::RelativePathElement& target) {
  Convert(source.reference_type_id(), target.reference_type_id);
  target.inverse = source.inverse();
  target.include_subtypes = source.include_subtypes();
  target.target_name = source.target_name();
}

void Convert(const scada::RelativePathElement& source,
             protocol::RelativePathElement& target) {
  Convert(source.reference_type_id, *target.mutable_reference_type_id());
  if (source.inverse)
    target.set_inverse(true);
  if (source.include_subtypes)
    target.set_include_subtypes(true);
  target.set_target_name(source.target_name.name());
}

void Convert(const protocol::BrowsePathResult& source,
             scada::BrowsePathResult& target) {
  target.status_code = static_cast<scada::StatusCode>(source.status_code());
  Convert(source.target(), target.targets);
}

void Convert(const scada::BrowsePathResult& source,
             protocol::BrowsePathResult& target) {
  target.set_status_code(static_cast<uint32_t>(source.status_code));
  Convert(source.targets, *target.mutable_target());
}

void Convert(const protocol::BrowsePathTarget& source,
             scada::BrowsePathTarget& target) {
  target.target_id = ConvertTo<scada::NodeId>(source.target_id());
  target.remaining_path_index = source.remaining_path_index();
}

void Convert(const scada::BrowsePathTarget& source,
             protocol::BrowsePathTarget& target) {
  assert(source.target_id.server_index() == 0);
  assert(source.target_id.namespace_uri().empty());
  Convert(source.target_id.node_id(), *target.mutable_target_id());
  target.set_remaining_path_index(source.remaining_path_index);
}

bool AssertValid(const scada::ModelChangeEvent& event) {
  assert(!event.node_id.is_null());
  assert(event.verb != 0);

  if (event.verb & scada::ModelChangeEvent::NodeAdded) {
    assert(!event.type_definition_id.is_null());
  }

  if (event.verb & scada::ModelChangeEvent::NodeDeleted) {
    assert(event.verb ==
           static_cast<uint8_t>(scada::ModelChangeEvent::NodeDeleted));
  }

  return true;
}

void Convert(const protocol::ModelChangeEvent& source,
             scada::ModelChangeEvent& target) {
  if (source.has_node_id())
    Convert(source.node_id(), target.node_id);
  if (source.has_type_definition_id())
    Convert(source.type_definition_id(), target.type_definition_id);

  if (source.node_added())
    target.verb |= scada::ModelChangeEvent::NodeAdded;
  if (source.node_deleted())
    target.verb |= scada::ModelChangeEvent::NodeDeleted;
  if (source.reference_added())
    target.verb |= scada::ModelChangeEvent::ReferenceAdded;
  if (source.reference_deleted())
    target.verb |= scada::ModelChangeEvent::ReferenceDeleted;

  assert(AssertValid(target));
}

void Convert(const scada::ModelChangeEvent& source,
             protocol::ModelChangeEvent& target) {
  assert(AssertValid(source));

  if (!source.node_id.is_null())
    Convert(source.node_id, *target.mutable_node_id());
  if (!source.type_definition_id.is_null())
    Convert(source.type_definition_id, *target.mutable_type_definition_id());

  if (source.verb & scada::ModelChangeEvent::NodeAdded)
    target.set_node_added(true);
  if (source.verb & scada::ModelChangeEvent::NodeDeleted)
    target.set_node_deleted(true);
  if (source.verb & scada::ModelChangeEvent::ReferenceAdded)
    target.set_reference_added(true);
  if (source.verb & scada::ModelChangeEvent::ReferenceDeleted)
    target.set_reference_deleted(true);
}

void Convert(const protocol::AggregateFilter& source,
             scada::AggregateFilter& target) {
  target.start_time = scada::DateTime::FromInternalValue(source.start_time());
  target.interval = scada::Duration::FromInternalValue(source.interval());
  Convert(source.aggregate_type(), target.aggregate_type);
}

void Convert(const scada::AggregateFilter& source,
             protocol::AggregateFilter& target) {
  target.set_start_time(source.start_time.ToInternalValue());
  target.set_interval(source.interval.ToInternalValue());
  Convert(std::move(source.aggregate_type), *target.mutable_aggregate_type());
}

void Convert(const protocol::MonitoringParameters& source,
             scada::MonitoringParameters& target) {
  if (source.has_aggregate_filter())
    target.filter =
        ConvertTo<scada::AggregateFilter>(source.aggregate_filter());
  else if (source.has_event_filter())
    target.filter = ConvertTo<scada::EventFilter>(source.event_filter());
}

void Convert(const scada::MonitoringParameters& source,
             protocol::MonitoringParameters& target) {
  if (auto* aggregate_filter =
          std::get_if<scada::AggregateFilter>(&source.filter)) {
    Convert(*aggregate_filter, *target.mutable_aggregate_filter());
  } else if (auto* event_filter =
                 std::get_if<scada::EventFilter>(&source.filter)) {
    Convert(*event_filter, *target.mutable_event_filter());
  }
}

void Convert(const protocol::ExpandedNodeId& source,
             scada::ExpandedNodeId& target) {
  target = ConvertTo<scada::NodeId>(source.node_id());
}

void Convert(const scada::ExpandedNodeId& source,
             protocol::ExpandedNodeId& target) {
  Convert(source.node_id(), *target.mutable_node_id());
}

void Convert(const protocol::AddReference& source,
             scada::AddReferencesItem& target) {
  Convert(source.source_node_id(), target.source_node_id);
  Convert(source.reference_type_id(), target.reference_type_id);
  target.forward = source.forward();
  Convert(source.target_node_id(), target.target_node_id);
  Convert(source.target_server_uri(), target.target_server_uri);
  Convert(source.target_node_class(), target.target_node_class);
}

void Convert(const scada::AddReferencesItem& source,
             protocol::AddReference& target) {
  Convert(source.source_node_id, *target.mutable_source_node_id());
  Convert(source.reference_type_id, *target.mutable_reference_type_id());
  target.set_forward(source.forward);
  Convert(source.target_node_id, *target.mutable_target_node_id());
  Convert(source.target_server_uri, *target.mutable_target_server_uri());
  target.set_target_node_class(
      ConvertTo<protocol::NodeClass>(source.target_node_class));
}

void Convert(const protocol::DeleteReference& source,
             scada::DeleteReferencesItem& target) {
  Convert(source.source_node_id(), target.source_node_id);
  Convert(source.reference_type_id(), target.reference_type_id);
  target.forward = source.forward();
  Convert(source.target_node_id(), target.target_node_id);
  target.delete_bidirectional = source.delete_bidirectional();
}

void Convert(const scada::DeleteReferencesItem& source,
             protocol::DeleteReference& target) {
  Convert(source.source_node_id, *target.mutable_source_node_id());
  Convert(source.reference_type_id, *target.mutable_reference_type_id());
  target.set_forward(source.forward);
  Convert(source.target_node_id, *target.mutable_target_node_id());
  target.set_delete_bidirectional(source.delete_bidirectional);
}

void Convert(const protocol::AddNode& source, scada::AddNodesItem& target) {
  if (source.has_requested_node_id())
    Convert(source.requested_node_id(), target.requested_id);
  Convert(source.parent_id(), target.parent_id);
  target.node_class = static_cast<scada::NodeClass>(source.node_class());
  Convert(source.type_definition_id(), target.type_definition_id);
  if (source.has_attributes())
    Convert(source.attributes(), target.attributes);
}

void Convert(const scada::AddNodesItem& source, protocol::AddNode& target) {
  target.set_node_class(ConvertTo<protocol::NodeClass>(source.node_class));
  if (!source.requested_id.is_null())
    Convert(source.requested_id, *target.mutable_requested_node_id());
  Convert(source.parent_id, *target.mutable_parent_id());
  Convert(source.type_definition_id, *target.mutable_type_definition_id());
  if (!source.attributes.empty())
    Convert(std::move(source.attributes), *target.mutable_attributes());
}

void Convert(const protocol::AddNodeResult& source,
             scada::AddNodesResult& target) {
  target.status_code = static_cast<scada::StatusCode>(source.status_code());
  target.added_node_id = source.has_added_node_id()
                             ? ConvertTo<scada::NodeId>(source.added_node_id())
                             : scada::NodeId{};
}

void Convert(const scada::AddNodesResult& source,
             protocol::AddNodeResult& target) {
  target.set_status_code(
      static_cast<google::protobuf::uint32>(source.status_code));

  if (!source.added_node_id.is_null())
    Convert(source.added_node_id, *target.mutable_added_node_id());
}

void Convert(const protocol::DeleteNode& source,
             scada::DeleteNodesItem& target) {
  Convert(source.node_id(), target.node_id);
  target.delete_target_references = source.delete_target_references();
}

void Convert(const scada::DeleteNodesItem& source,
             protocol::DeleteNode& target) {
  Convert(source.node_id, *target.mutable_node_id());
  if (target.delete_target_references())
    target.set_delete_target_references(true);
}
