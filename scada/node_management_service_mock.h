#include "scada/node_management_service.h"

#include <gmock/gmock.h>

namespace scada {

class MockNodeManagementService : public NodeManagementService {
 public:
  MOCK_METHOD(void,
              AddNodes,
              (const std::vector<AddNodesItem>& inputs,
               const AddNodesCallback& callback));

  MOCK_METHOD(void,
              DeleteNodes,
              (const std::vector<DeleteNodesItem>& inputs,
               const DeleteNodesCallback& callback));

  MOCK_METHOD(void,
              ChangeUserPassword,
              (const NodeId& user_node_id,
               const LocalizedText& current_password,
               const LocalizedText& new_password,
               const StatusCallback& callback));

  MOCK_METHOD(void,
              AddReferences,
              (const std::vector<AddReferencesItem>& inputs,
               const AddReferencesCallback& callback));

  MOCK_METHOD(void,
              DeleteReferences,
              (const std::vector<DeleteReferencesItem>& inputs,
               const DeleteReferencesCallback& callback));
};

}  // namespace scada
