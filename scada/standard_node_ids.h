#pragma once

#include "scada/node_id.h"

namespace scada {
namespace id {

constexpr NumericId BaseDataType = 24;
constexpr NumericId Boolean = 1;
constexpr NumericId Int8 = 2;
constexpr NumericId UInt8 = 3;
constexpr NumericId Int16 = 4;
constexpr NumericId UInt16 = 5;
constexpr NumericId Int32 = 6;
constexpr NumericId UInt32 = 7;
constexpr NumericId Int64 = 8;
constexpr NumericId UInt64 = 9;
constexpr NumericId Double = 11;
constexpr NumericId ByteString = 15;
constexpr NumericId String = 12;
constexpr NumericId QualifiedName = 20;
constexpr NumericId LocalizedText = 21;
constexpr NumericId NodeId = 17;
constexpr NumericId ExpandedNodeId = 18;
constexpr NumericId DateTime = 13;
constexpr NumericId Enumeration = 29;

constexpr NumericId References = 31;
constexpr NumericId NonHierarchicalReferences = 32;
constexpr NumericId HierarchicalReferences = 33;
constexpr NumericId Aggregates = 44;
constexpr NumericId HasComponent = 47;
constexpr NumericId HasProperty = 46;
constexpr NumericId HasChild = 34;
constexpr NumericId Organizes = 35;
constexpr NumericId HasTypeDefinition = 40;
constexpr NumericId HasSubtype = 45;
constexpr NumericId HasModellingRule = 37;
constexpr NumericId HasEventSource = 36;
constexpr NumericId HasNotifier = 48;
// Historized node (source) -> its historical configuration object (target),
// conventionally browse-named "HA Configuration"; subtype of Aggregates
// (OPC UA Part 11 §5.3.2 / §5.2.4,
// https://reference.opcfoundation.org/Core/Part11/v105/docs/5.3.2). The
// historian's external-node historization entries are the targets of this
// reference (ADR 0002 §2c).
constexpr NumericId HasHistoricalConfiguration = 56;

constexpr NumericId BaseObjectType = 58;
constexpr NumericId BaseVariableType = 62;
constexpr NumericId FolderType = 61;
constexpr NumericId PropertyType = 68;
// The per-node historical configuration ObjectType (OPC UA Part 11 §5.2.2,
// https://reference.opcfoundation.org/Core/Part11/v105/docs/5.2.2).
constexpr NumericId HistoricalDataConfigurationType = 2318;

constexpr NumericId RootFolder = 84;
constexpr NumericId ObjectsFolder = 85;
constexpr NumericId TypesFolder = 86;
constexpr NumericId Server = 2253;
constexpr NumericId Server_ServerArray = 2254;
constexpr NumericId Server_NamespaceArray = 2255;
constexpr NumericId Server_ServerStatus = 2256;
constexpr NumericId Server_ServerStatus_CurrentTime = 2258;
constexpr NumericId Server_ServerStatus_State = 2259;
constexpr NumericId Server_ServiceLevel = 2267;
constexpr NumericId Server_Auditing = 2994;
constexpr NumericId Server_ServerCapabilities = 2268;
constexpr NumericId Server_ServerCapabilities_ServerProfileArray = 2269;
constexpr NumericId Server_ServerCapabilities_LocaleIdArray = 2271;
constexpr NumericId Server_ServerCapabilities_MinSupportedSampleRate = 2272;
constexpr NumericId Server_ServerCapabilities_MaxBrowseContinuationPoints =
    2735;
constexpr NumericId Server_ServerCapabilities_OperationLimits = 11704;
// RoleSet: the roles the server supports, published under ServerCapabilities
// (OPC UA Part 18 §4.3,
// https://reference.opcfoundation.org/Core/Part18/v105/docs/4.3). Ids per the
// official 1.05 NodeIds.csv.
constexpr NumericId Server_ServerCapabilities_RoleSet = 15606;
constexpr NumericId Server_ServerCapabilities_RoleSet_AddRole = 16301;
constexpr NumericId Server_ServerCapabilities_RoleSet_RemoveRole = 16304;
// Server.Namespaces and the NamespaceMetadataType of its per-namespace
// metadata objects (OPC UA Part 5 §6.3.12/§6.3.13,
// https://reference.opcfoundation.org/Core/Part5/v105/docs/6.3.12). Ids per
// the official 1.05 NodeIds.csv.
constexpr NumericId Server_Namespaces = 11715;
constexpr NumericId NamespaceMetadataType = 11616;
// The RolePermissionType structure (OPC UA Part 3 §8.56) carried by the
// NamespaceMetadata DefaultRolePermissions property.
constexpr NumericId RolePermissionType = 96;
constexpr NumericId RoleSetType = 15607;
constexpr NumericId RoleType = 15620;
// The IdentityMappingRuleType structure (OPC UA Part 18 §4.4.3) and its
// DefaultBinary encoding object.
constexpr NumericId IdentityMappingRuleType = 15634;
constexpr NumericId IdentityMappingRuleType_Encoding_DefaultBinary = 15736;
constexpr NumericId OperationLimits_MaxNodesPerRead = 11705;
constexpr NumericId OperationLimits_MaxNodesPerWrite = 11707;
constexpr NumericId OperationLimits_MaxNodesPerMethodCall = 11709;
constexpr NumericId OperationLimits_MaxNodesPerBrowse = 11710;
constexpr NumericId OperationLimits_MaxNodesPerRegisterNodes = 11711;
constexpr NumericId OperationLimits_MaxNodesPerTranslateBrowsePathsToNodeIds =
    11712;
constexpr NumericId OperationLimits_MaxNodesPerNodeManagement = 11713;
constexpr NumericId OperationLimits_MaxNodesPerHistoryReadData = 12165;
constexpr NumericId OperationLimits_MaxNodesPerHistoryReadEvents = 12166;
constexpr NumericId OperationLimits_MaxMonitoredItemsPerCall = 11714;

constexpr NumericId ModellingRules = 87;
constexpr NumericId ModellingRule_Mandatory = 78;

constexpr NumericId BaseEventType = 2041;
constexpr NumericId SystemEventType = 2130;
constexpr NumericId GeneralModelChangeEventType = 2133;
constexpr NumericId SemanticChangeEventType = 2738;

// Audit event types (OPC UA Part 5 §6.4). Emitted for security-relevant actions
// per the auditability objective (Part 2 §4.14),
// https://reference.opcfoundation.org/Core/Part5/v105/docs/6.4
constexpr NumericId AuditEventType = 2052;
constexpr NumericId AuditSecurityEventType = 2058;
constexpr NumericId AuditSessionEventType = 2069;
constexpr NumericId AuditCreateSessionEventType = 2071;
constexpr NumericId AuditActivateSessionEventType = 2075;
constexpr NumericId AuditWriteUpdateEventType = 2100;

constexpr NumericId AggregateFunction_Average = 2342;
constexpr NumericId AggregateFunction_Total = 2344;
constexpr NumericId AggregateFunction_Minimum = 2346;
constexpr NumericId AggregateFunction_Maximum = 2347;
constexpr NumericId AggregateFunction_Count = 2352;
constexpr NumericId AggregateFunction_Start = 2357;
constexpr NumericId AggregateFunction_End = 2358;

constexpr NumericId AcknowledgeableConditionType_Acknowledge = 9111;

}  // namespace id
}  // namespace scada
