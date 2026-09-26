#pragma once

#include "remote/message_sender.h"
#include "remote/protocol.h"

#include <utility>
#include <vector>

// Records every message and request a component under test sends, so a test
// asserts on what went out on the wire rather than on the calls that sent it.
//
// A request is kept together with its response handler, so a test plays the
// peer by answering it: `fake.requests().back().response_handler(response)`.
// Nothing is answered on its own -- an unanswered request stays pending, which
// is exactly the state a test of an in-flight request needs.
//
// Both production implementations (SessionProxy and SessionStub) are whole
// session endpoints that need a transport and a peer, so neither can stand in
// for the channel seam these tests exercise; this is the shared fake instead
// of a per-test mock.
class MessageSenderFake : public MessageSender {
 public:
  // A request as it was sent, and the handler that completes it.
  struct SentRequest {
    protocol::Request request;
    ResponseHandler response_handler;
  };

  void Send(protocol::Message& message) override {
    sent_messages_.push_back(message);
  }

  void Request(protocol::Request& request,
               ResponseHandler response_handler) override {
    requests_.push_back({request, std::move(response_handler)});
  }

  // Every message passed to Send(), oldest first.
  const std::vector<protocol::Message>& sent_messages() const {
    return sent_messages_;
  }

  // Every request passed to Request(), oldest first.
  const std::vector<SentRequest>& requests() const { return requests_; }

 private:
  std::vector<protocol::Message> sent_messages_;
  std::vector<SentRequest> requests_;
};
