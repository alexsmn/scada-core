#pragma once

#include "remote/message_sender.h"
#include "remote/protocol.h"

#include <gmock/gmock.h>

class MessageSenderMock : public MessageSender {
 public:
  MOCK_METHOD1(Send, void(protocol::Message& message));
  MOCK_METHOD2(Request,
               void(protocol::Request& request,
                    ResponseHandler response_handler));
};
