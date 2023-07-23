#pragma once

#include "base/debug_util.h"
#include "scada/attribute_ids.h"
#include "scada/localized_text.h"
#include "scada/node_id.h"
#include "scada/qualified_name.h"
#include "scada/status.h"
#include "scada/variant.h"

namespace scada {

class AttributeSet {
 public:
  void Add(AttributeId id) { bits_ |= 1 << static_cast<unsigned>(id); }
  void Remove(AttributeId id) { bits_ &= ~(1 << static_cast<unsigned>(id)); }

  bool has(AttributeId id) const {
    return (bits_ & (1 << static_cast<unsigned>(id))) != 0;
  }

  bool empty() const { return bits_ == 0; }

 private:
  unsigned bits_ = 0;
};

struct NodeAttributes {
  NodeAttributes& set_browse_name(QualifiedName browse_name) {
    this->browse_name = std::move(browse_name);
    return *this;
  }

  NodeAttributes& set_display_name(LocalizedText display_name) {
    this->display_name = std::move(display_name);
    return *this;
  }

  NodeAttributes& set_data_type(NodeId data_type) {
    this->data_type = std::move(data_type);
    return *this;
  }

  NodeAttributes& set_value(Variant value) {
    this->value = std::move(value);
    return *this;
  }

  bool empty() const {
    return browse_name.empty() && display_name.empty() && data_type.is_null() &&
           !value.has_value();
  }

  WARN_UNUSED_RESULT std::optional<scada::Variant> Get(
      AttributeId attribute_id) const;
  WARN_UNUSED_RESULT scada::StatusCode Set(scada::AttributeId attribute_id,
                                           scada::Variant value);

  void Update(scada::NodeAttributes&& updated_attributes);

  QualifiedName browse_name;
  LocalizedText display_name;
  NodeId data_type;
  std::optional<Variant> value;
};

inline std::optional<scada::Variant> NodeAttributes::Get(
    AttributeId attribute_id) const {
  switch (attribute_id) {
    case AttributeId::BrowseName:
      return browse_name;
    case AttributeId::DisplayName:
      return display_name;
    case AttributeId::DataType:
      return data_type;
    case AttributeId::Value:
      return value.value_or(scada::Variant{});
    default:
      return std::nullopt;
  }
}

inline scada::StatusCode NodeAttributes::Set(scada::AttributeId attribute_id,
                                             scada::Variant value) {
  switch (attribute_id) {
    case scada::AttributeId::BrowseName: {
      auto* typed_value = value.get_if<scada::QualifiedName>();
      if (!typed_value)
        return scada::StatusCode::Bad;
      browse_name = std::move(*typed_value);
      return scada::StatusCode::Good;
    }

    case scada::AttributeId::DisplayName: {
      auto* typed_value = value.get_if<scada::LocalizedText>();
      if (!typed_value)
        return scada::StatusCode::Bad;
      display_name = std::move(*typed_value);
      return scada::StatusCode::Good;
    }

    case scada::AttributeId::DataType: {
      auto* typed_value = value.get_if<scada::NodeId>();
      if (!typed_value)
        return scada::StatusCode::Bad;
      data_type = std::move(*typed_value);
      return scada::StatusCode::Good;
    }

    case scada::AttributeId::Value:
      value = std::move(value);
      return scada::StatusCode::Good;

    default:
      return scada::StatusCode::Bad_WrongAttributeId;
  }
}

inline void NodeAttributes::Update(scada::NodeAttributes&& updated_attributes) {
  if (!updated_attributes.browse_name.empty())
    browse_name = std::move(updated_attributes.browse_name);

  if (!updated_attributes.display_name.empty())
    display_name = std::move(updated_attributes.display_name);

  if (!updated_attributes.data_type.is_null())
    data_type = std::move(updated_attributes.data_type);

  if (updated_attributes.value.has_value())
    value = std::move(updated_attributes.value);
}

inline bool operator==(const NodeAttributes& a, const NodeAttributes& b) {
  return a.browse_name == b.browse_name && a.display_name == b.display_name &&
         a.data_type == b.data_type && a.value == b.value;
}

inline bool operator!=(const NodeAttributes& a, const NodeAttributes& b) {
  return !operator==(a, b);
}

inline std::ostream& operator<<(std::ostream& stream,
                                const NodeAttributes& attributes) {
  using ::operator<<;
  return stream << "{"
                << "browse_name: " << attributes.browse_name << ", "
                << "display_name: " << attributes.display_name << ", "
                << "data_type: " << attributes.data_type << ", "
                << "value: " << attributes.value << "}";
}

}  // namespace scada