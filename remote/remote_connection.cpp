#include "remote/remote_connection.h"

#include "remote/protocol.h"
#include "remote/protocol_utils.h"
#include "remote/session_stub.h"

#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <stdexcept>

ServerConnection::ServerConnection(ServerConnectionContext&& context)
    : ServerConnectionContext{std::move(context)} {
  boost::asio::co_spawn(transport_->GetExecutor(), Run(),
                        boost::asio::detached);
}

ServerConnection::~ServerConnection() {
  if (session_)
    session_->SetConnection(nullptr);
}

net::awaitable<void> ServerConnection::Run() {
  auto open_result = co_await transport_->Open(
      {.on_close = [this](net::Error error) { OnTransportClosed(error); }});

  if (open_result != net::OK) {
    Close();
    co_return;
  }

  auto cancelation = std::weak_ptr{cancelation_};
  std::vector<char> message;

  while (!cancelation.expired()) {
    // TODO: Revise buffer.
    message.resize(1024 * 1024);
    auto bytes_read = co_await transport_->Read(message);

    if (cancelation.expired()) {
      co_return;
    }

    if (!bytes_read.ok() || *bytes_read == 0) {
      Close();
      co_return;
    }

    message.resize(*bytes_read);

    OnTransportMessageReceived(message);
  }
}

void ServerConnection::OnSessionDeleted() {
  session_ = nullptr;
}

void ServerConnection::OnTransportMessageReceived(std::span<const char> data) {
  protocol::Message message;
  if (!message.ParseFromArray(data.data(), static_cast<int>(data.size()))) {
    Close();
    return;
  }

  std::weak_ptr<bool> cancelation = cancelation_;

  for (auto& request : message.requests()) {
    if (request.has_create_session())
      OnCreateSession(request);
    if (cancelation.expired())
      return;
    if (request.has_delete_session())
      OnDeleteSession(request);
    if (cancelation.expired())
      return;
  }

  if (session_)
    session_->ProcessMessage(message);
}

void ServerConnection::Close() {
  delete this;
}

void ServerConnection::OnTransportClosed(net::Error error) {
  Close();
}

void ServerConnection::Send(protocol::Message& message) {
  std::string string;
  if (!message.AppendToString(&string)) {
    throw std::runtime_error("Can't serialize the message");
  }

  boost::asio::co_spawn(
      transport_->GetExecutor(),
      [this, string = std::move(string)]() -> net::awaitable<void> {
        auto bytes_written = co_await write_queue_.Write(string);
        if (!bytes_written.ok() || *bytes_written != string.size()) {
          Close();
          co_return;
        }
      },
      boost::asio::detached);
}

void ServerConnection::OnCreateSession(const protocol::Request& request) {
  assert(request.has_create_session());

  auto request_id = request.request_id();
  create_session_handler_(request, [this, request_id](
                                       const CreateSessionResult& result) {
    protocol::Message message;
    auto& response = *message.add_responses();
    response.set_request_id(request_id);
    auto& create_session_result = *response.mutable_create_session_result();
    create_session_result.set_protocol_version_major(
        result.protocol_version_major);
    create_session_result.set_protocol_version_minor(
        result.protocol_version_minor);
    Convert(result.status, *response.mutable_status());
    if (result.status) {
      Convert(result.user_id, *create_session_result.mutable_user_node_id());
      create_session_result.set_user_rights(result.user_rights);
    }
    Send(message);

    session_ = result.session;
    if (session_)
      session_->SetConnection(this);
  });
}

void ServerConnection::OnDeleteSession(const protocol::Request& request) {
  assert(request.has_delete_session());

  scada::Status status(scada::StatusCode::Good);

  if (session_) {
    delete_session_handler_(*session_);
    session_ = nullptr;
  } else {
    status = scada::Status(scada::StatusCode::Bad_SessionIsLoggedOff);
  }

  protocol::Message message;
  auto& response = *message.add_responses();
  response.set_request_id(request.request_id());
  Convert(status, *response.mutable_status());

  try {
    Send(message);
  } catch (const std::exception&) {
  }
}
