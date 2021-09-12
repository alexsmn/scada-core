#pragma once

namespace protocol {
const int PROTOCOL_VERSION_MAJOR = 18;
const int PROTOCOL_VERSION_MINOR = 0;
}  // namespace protocol

#pragma warning(push, 1)
#include "scada.grpc.pb.h"
#pragma warning(pop)
