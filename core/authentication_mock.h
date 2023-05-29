#pragma once

#include "core/authentication.h"

#include <gmock/gmock.h>

namespace scada {

using MockAuthenticator = testing::MockFunction<promise<AuthenticationResult>(
    const LocalizedText& user_name,
    const LocalizedText& password)>;

}