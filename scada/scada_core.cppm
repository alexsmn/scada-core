// scada.core — named C++20 module facade over the core/scada headers.
//
// Same design and rules as scada.base (see base/scada_base.cppm and
// docs/cxx-modules.md): headers stay the source of truth, the global module
// fragment includes them, the purview re-exports names with `export using`.
// `export import` of scada.base / scada.metrics mirrors scada_core's PUBLIC
// links, so `import scada.core;` provides the full transitive surface.
//
// Not exported (documented; textual #include alongside the import):
//  - the LOCALIZED_TEXT macro (scada/localized_text.h) and
//    REGISTER_DATA_SERVICES (scada/data_services_factory.h);
//  - the scada::id constants (scada/standard_node_ids.h) - namespace-scope
//    constexpr => internal linkage, ill-formed to export;
//  - global-namespace ::operator<< overloads (e.g. StatusOr streaming in
//    status_or.h) - exporting ::operator<< would drag the whole global
//    overload set (boost_log/debug_util streams), which scada.base
//    deliberately avoids. The scada:: namespace operator sets ARE exported.

module;

// ---- Global module fragment: headers stay the source of truth ----
#include "scada/access_rights.h"
#include "scada/aggregate_filter.h"
#include "scada/attribute_ids.h"
#include "scada/attribute_service.h"
#include "scada/authentication.h"
#include "scada/authentication_adapters.h"
#include "scada/authorization.h"
#include "scada/basic_types.h"
#include "scada/callback_awaitable.h"
#include "scada/client.h"
#include "scada/client_monitored_item.h"
#include "scada/data_change_filter.h"
#include "scada/data_services.h"
#include "scada/data_services_factory.h"
#include "scada/data_value.h"
#include "scada/date_time.h"
#include "scada/date_time_range.h"
#include "scada/event.h"
#include "scada/event_filter.h"
#include "scada/event_util.h"
#include "scada/expanded_node_id.h"
#include "scada/extension_object.h"
#include "scada/history_service.h"
#include "scada/history_types.h"
#include "scada/history_update_service.h"
#include "scada/identity_mapping_rule_encoding.h"
#include "scada/item_factory_subscription.h"
#include "scada/legacy_monitored_item_adapter.h"
#include "scada/localized_text.h"
#include "scada/logging.h"
#include "scada/method_service.h"
#include "scada/method_util.h"
#include "scada/monitored_item.h"
#include "scada/monitored_item_service.h"
#include "scada/monitored_item_service_awaitable.h"
#include "scada/monitored_item_subscription_pump.h"
#include "scada/monitoring_parameters.h"
#include "scada/multiplexing_monitored_item_subscription.h"
#include "scada/node.h"
#include "scada/node_attributes.h"
#include "scada/node_class.h"
#include "scada/node_id.h"
#include "scada/node_management_service.h"
#include "scada/qualified_name.h"
#include "scada/qualified_name_util.h"
#include "scada/qualifier.h"
#include "scada/read_value_id.h"
#include "scada/role_permission_encoding.h"
#include "scada/service_context.h"
#include "scada/services.h"
#include "scada/serving_gate.h"
#include "scada/session.h"
#include "scada/session_debugger.h"
#include "scada/session_service.h"
#include "scada/shared_value.h"
#include "scada/standard_node_ids.h"
#include "scada/status.h"
#include "scada/status_callback.h"
#include "scada/status_or.h"
#include "scada/string.h"
#include "scada/tvq.h"
#include "scada/validation.h"
#include "scada/variant.h"
#include "scada/variant_utils.h"
#include "scada/view_events.h"
#include "scada/view_service.h"
#include "scada/write_flags.h"

export module scada.core;

// Mirror scada_core's PUBLIC link transitivity.
export import scada.base;
export import scada.metrics;

// Keep the GMF-attached std specializations decl-reachable for import-only
// TUs (nothing else names them, so they could otherwise be discarded).
static_assert(sizeof(std::hash<scada::NodeId>) > 0);
static_assert(sizeof(std::formatter<scada::NodeId>) > 0);
static_assert(sizeof(std::formatter<scada::Event>) > 0);
static_assert(sizeof(std::formatter<scada::ModelChangeEvent>) > 0);
static_assert(sizeof(std::formatter<scada::SemanticChangeEvent>) > 0);

export namespace scada {

// basic_types.h / string.h
using scada::Boolean;
using scada::ByteString;
using scada::Double;
using scada::Int16;
using scada::Int32;
using scada::Int64;
using scada::Int8;
using scada::NamespaceIndex;
using scada::String;
using scada::UInt16;
using scada::UInt32;
using scada::UInt64;
using scada::UInt8;

// node_id.h / expanded_node_id.h
using scada::ExpandedNodeId;
using scada::NodeId;
using scada::NodeIdType;
using scada::NumericId;

// qualified_name.h / qualifier.h / localized_text.h
using scada::LocalizedText;
using scada::QualifiedName;
using scada::Qualifier;
using scada::ToLocalizedText;

// variant.h / data_value.h / tvq.h / extension_object.h / shared_value.h
using scada::DataValue;
using scada::ExtensionObject;
using scada::IsUpdate;
using scada::ParseBuiltInType;
using scada::ToBuiltInDataType;
using scada::ToNodeId;
using scada::Variant;
using scada::VQ;

// date_time.h / date_time_range.h
using scada::DateTime;
using scada::DateTimeRange;
using scada::Duration;

// status.h / status_or.h / status_callback.h
using scada::MultiStatusCallback;
using scada::Status;
using scada::StatusCallback;
using scada::StatusCode;
using scada::StatusLimit;
using scada::StatusOr;
using scada::StatusSeverity;

// attribute_ids.h / node_class.h / node_attributes.h
using scada::AttributeId;
using scada::AttributeSet;
using scada::IsInstance;
using scada::IsTypeDefinition;
using scada::NodeAttributes;
using scada::NodeClass;

// read_value_id.h / write_flags.h
using scada::ReadValueId;
using scada::WriteFlags;

// monitoring_parameters.h / monitored_item.h
using scada::DataChangeHandler;
using scada::DataChangeNotification;
using scada::EventHandler;
using scada::EventNotification;
using scada::ItemStatusNotification;
using scada::MonitoredItem;
using scada::MonitoredItemCreateRequest;
using scada::MonitoredItemCreateResult;
using scada::MonitoredItemId;
using scada::MonitoredItemNotification;
using scada::MonitoredItemSubscription;
using scada::MonitoredItemSubscriptionOptions;
using scada::MonitoringFilter;
using scada::MonitoringParameters;
using scada::OverflowNotification;

// services.h + the service interface headers
using scada::AttributeService;
using scada::HistoryService;
using scada::HistoryUpdateService;
using scada::MethodService;
using scada::MonitoredItemService;
using scada::NodeManagementService;
using scada::services;
using scada::SessionService;
using scada::ViewService;
using scada::WriteValue;

// history_types.h
using scada::AcknowledgeCallback;
using scada::HistoryReadEventsDetails;
using scada::HistoryReadEventsResult;
using scada::HistoryReadRawDetails;
using scada::HistoryReadRawResult;
using scada::PerformUpdateType;
using scada::UpdateDataDetails;
using scada::UpdateEventDetails;

// view_service.h / view_events.h
using scada::BrowseDescription;
using scada::BrowseDirection;
using scada::BrowsePath;
using scada::BrowsePathResult;
using scada::BrowsePathTarget;
using scada::BrowseResult;
using scada::BrowseResultMask;
using scada::ReferenceDescription;
using scada::ReferenceDescriptions;
using scada::RelativePath;
using scada::RelativePathElement;
using scada::ViewEvents;

// node_management_service.h
using scada::AddNodesItem;
using scada::AddNodesResult;
using scada::AddReferencesItem;
using scada::DeleteNodesItem;
using scada::DeleteReferencesItem;

// session_service.h / session_debugger.h / session.h
using scada::session;
using scada::SessionConnectParams;
using scada::SessionDebugger;
using scada::SessionSecuritySettings;

// client.h / node.h / service_context.h / logging.h
using scada::client;
using scada::node;
using scada::ServiceContext;
using scada::ServiceLogParams;

// callback_awaitable.h
using scada::AwaitCallbackTuple;
using scada::AwaitCallbackValue;
using scada::AwaitStatusCallback;
using scada::AwaitStatusCodesCallback;
using scada::AwaitStatusOrCallback;
using scada::CompleteStatusOrCallback;

// event.h / event_filter.h / event_util.h
using scada::AssembleEvent;
using scada::DisassembleEvent;
using scada::Event;
using scada::EventFilter;
using scada::EventSeverity;
using scada::ModelChangeEvent;
using scada::SemanticChangeEvent;

// aggregate_filter.h / data_change_filter.h
using scada::AggregateFilter;
using scada::DataChangeFilter;

// authentication.h / authentication_adapters.h
using scada::AuthenticationCallback;
using scada::AuthenticationResult;
using scada::Authenticator;
using scada::CoroutineAuthenticator;
using scada::FunctionCoroutineAuthenticator;

// authorization.h / access_rights.h
using scada::AccessRight;
using scada::AccessRightBit;
using scada::HasAccessRight;
using scada::IdentityCriteriaType;
using scada::IdentityMappingRule;
using scada::Permission;
using scada::RolePermissionType;
using scada::WellKnownRole;
namespace access_level {
using scada::access_level::kCurrentRead;
using scada::access_level::kCurrentWrite;
using scada::access_level::kHistoryRead;
using scada::access_level::kHistoryWrite;
using scada::access_level::kNone;
using scada::access_level::kSemanticChange;
using scada::access_level::kStatusWrite;
using scada::access_level::kTimestampWrite;
}  // namespace access_level

// identity_mapping_rule_encoding.h / role_permission_encoding.h
// (the inline constexpr *Id constants have external linkage - exportable)
using scada::DecodeIdentityMappingRule;
using scada::EncodeIdentityMappingRules;
using scada::EncodeRolePermissions;
using scada::kIdentityMappingRuleTypeDataTypeId;
using scada::kIdentityMappingRuleTypeDefaultBinaryId;
using scada::kRolePermissionTypeDataTypeId;
using scada::MakeIdentityMappingRuleObject;

// method_util.h / qualified_name_util.h / variant_utils.h
// (the scada::internal helpers are deliberately not exported)
using scada::ConvertVariant;
using scada::InvokeMethod;
using scada::ToQualifiedName;

// client_monitored_item.h / item_factory_subscription.h /
// legacy_monitored_item_adapter.h / monitored_item_service_awaitable.h /
// monitored_item_subscription_pump.h /
// multiplexing_monitored_item_subscription.h
using scada::LegacyMonitoredItemAdapter;
using scada::MakeItemFactorySubscription;
using scada::MakeMultiplexingMonitoredItemSubscription;
using scada::monitored_item;
using scada::MonitoredItemFactory;
using scada::MonitoredItemRoute;
using scada::MonitoredItemRouter;
using scada::MonitoredItemSubscriptionPump;
using scada::ReadInitialValueAsync;
using scada::ReadInitialValuesAsync;
using scada::ServingGate;

// Free operator sets at scada namespace scope, picked up across all
// included headers: NodeId/NumericId comparisons (node_id.h) and the
// ostream << overloads for StatusCode/Status/AttributeId/Qualifier etc.
using scada::operator==;
using scada::operator!=;
using scada::operator<<;

}  // namespace scada

export {
  // shared_value.h
  using ::SharedValue;

  // validation.h
  using ::Validate;
  using ::ValidateUnique;

  // data_services.h / data_services_factory.h (global-namespace
  // declarations; the REGISTER_DATA_SERVICES macro is not exportable -
  // include scada/data_services_factory.h for it)
  using ::CreateDataServices;
  using ::DataServices;
  using ::DataServicesContext;
  using ::DataServicesFactoryMethod;
  using ::DataServicesInfo;
  using ::DataServicesInfoList;
  using ::GetDataServicesInfoList;
  using ::RegisterDataServices;

  // The global ToString/ToString16 overload sets spread across scada
  // headers (status.h, variant.h, attribute_ids.h, node_class.h,
  // qualifier.h, localized_text.h, date_time.h, ...). Exported once, after
  // all GMF includes, so the full overload sets are captured.
  using ::ToString;
  using ::ToString16;
}  // export
