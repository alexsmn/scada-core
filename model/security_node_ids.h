#pragma once

#include "model/namespaces.h"
#include "scada/node_id.h"

namespace security {

namespace numeric_id {

const scada::NumericId Users = 29;

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
