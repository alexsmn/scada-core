#pragma once

#include "scada/authentication.h"
#include "scada/co_result.h"

#include <gmock/gmock.h>

namespace scada {

using MockAuthenticator = testing::MockFunction<CoStatusOr<
    AuthenticationResult>(LocalizedText user_name, LocalizedText password)>;

}
