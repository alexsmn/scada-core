#pragma once

#include "scada/authentication.h"

#include <gmock/gmock.h>

namespace scada {

using MockAuthenticator = testing::MockFunction<
    Awaitable<StatusOr<AuthenticationResult>>(LocalizedText user_name,
                                              LocalizedText password)>;

}
