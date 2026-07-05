#pragma once

#include "model/namespaces.h"
#include "scada/node_id.h"
#include "scada/standard_node_ids.h"

namespace security {

namespace numeric_id {

const scada::NumericId Users = 29;
const scada::NumericId Configurations = 362;

}  // namespace numeric_id

namespace id {

const scada::NodeId RootUser{234, NamespaceIndexes::SCADA};
const scada::NodeId Users{numeric_id::Users, NamespaceIndexes::SCADA};
const scada::NodeId UserType_AccessRights{170, NamespaceIndexes::SCADA};
const scada::NodeId UserType{16, NamespaceIndexes::SCADA};
const scada::NodeId UserType_ChangePassword{334, NamespaceIndexes::SCADA};
const scada::NodeId UserType_MultiSessions{345, NamespaceIndexes::SCADA};
const scada::NodeId UserType_ProfileJson{346, NamespaceIndexes::SCADA};
const scada::NodeId UserType_ProfileRevision{347, NamespaceIndexes::SCADA};
const scada::NodeId UserType_SaveProfile{348, NamespaceIndexes::SCADA};
// Method: verifies a user's password without opening a session. Admin-only
// inter-tier credential check used by remote-configuration tiers.
const scada::NodeId UserType_VerifyPassword{353, NamespaceIndexes::SCADA};

// User groups modeled as OPC UA Roles: a RoleType instance per group, its
// membership expressed as identity mapping rules with UserName criteria.
// OPC UA Part 18 §4.4.1 RoleType,
// https://reference.opcfoundation.org/Core/Part18/v105/docs/4.4
const scada::NodeId RoleType{354, NamespaceIndexes::SCADA};
// True when the group may author (add/delete nodes in) its configuration.
const scada::NodeId RoleType_Authoring{355, NamespaceIndexes::SCADA};
// Reference: the configuration (tenant) this role grants access to. The
// role→namespace-set binding is published as per-namespace
// DefaultRolePermissions (OPC UA Part 3 §5.2.9, Part 5 §6.3.13); this
// reference is the server-side source of truth those permissions are built
// from.
const scada::NodeId HasConfiguration{356, NamespaceIndexes::SCADA};
// One identity mapping rule row per group member.
// OPC UA Part 18 §4.4.3 IdentityMappingRuleType,
// https://reference.opcfoundation.org/Core/Part18/v105/docs/4.4.3
const scada::NodeId IdentityMappingRuleType{357, NamespaceIndexes::SCADA};
// IdentityCriteriaType enum value (OPC UA Part 18 §4.4.4).
const scada::NodeId IdentityMappingRuleType_CriteriaType{
    358, NamespaceIndexes::SCADA};
// Criteria string; for UserName rules, the user's BrowseName.
const scada::NodeId IdentityMappingRuleType_Criteria{359,
                                                     NamespaceIndexes::SCADA};
// A hosted configuration (tenant) on the central configuration server. There
// is no standard OPC UA object for "a hosted configuration"; the closest
// standard surface — a namespace set with NamespaceMetadata (OPC UA Part 5
// §6.3.12/§6.3.13) — is published per configuration by the config server.
const scada::NodeId ConfigurationType{360, NamespaceIndexes::SCADA};
// Methods: manage a role's identity mapping rules. The Identities property is
// read-only and modified only through these methods (OPC UA Part 18
// §4.4.5/§4.4.6,
// https://reference.opcfoundation.org/Core/Part18/v105/docs/4.4.5).
const scada::NodeId RoleType_AddIdentity{363, NamespaceIndexes::SCADA};
const scada::NodeId RoleType_RemoveIdentity{364, NamespaceIndexes::SCADA};
// Methods on the Configurations object: deploy/remove a hosted configuration
// (tenant) at runtime. Server-specific surface — OPC UA defines no standard
// service for creating a namespace set; the RESULT (NamespaceArray growth +
// NamespaceMetadata + DefaultRolePermissions) is fully standard (OPC UA
// Part 5 §6.3.1/§6.3.13,
// https://reference.opcfoundation.org/Core/Part5/v105/docs/6.3.13).
const scada::NodeId Configurations_DeployConfiguration{365,
                                                       NamespaceIndexes::SCADA};
const scada::NodeId Configurations_RemoveConfiguration{366,
                                                       NamespaceIndexes::SCADA};
// All roles — the well-known static objects and the custom (group) RoleType
// rows — are parented under Server.ServerCapabilities.RoleSet (OPC UA Part 18
// §4.3, https://reference.opcfoundation.org/Core/Part18/v105/docs/4.3).
const scada::NodeId RoleSet{scada::id::Server_ServerCapabilities_RoleSet,
                            NamespaceIndexes::NS0};
const scada::NodeId Configurations{numeric_id::Configurations,
                                   NamespaceIndexes::SCADA};

}  // namespace id

// Maps a numeric user id (the credential key, e.g. a UserCredentials row or a
// password.dat entry) back to the user's NodeId. The built-in root user is a
// static node in the SCADA namespace; all other users are UserType nodes in
// the USER namespace.
inline scada::NodeId UserNodeId(unsigned user_id) {
  if (user_id == id::RootUser.numeric_id()) {
    return id::RootUser;
  }
  return scada::NodeId{user_id, NamespaceIndexes::USER};
}

}  // namespace security
