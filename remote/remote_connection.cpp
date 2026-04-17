#include "remote/remote_connection.h"

#include "base/awaitable_promise.h"
#include "remote/protocol.h"
#include "remote/protocol_utils.h"
#include "remote/session_stub.h"

#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <stdexcept>

std::shared_ptr<ServerConnection> ServerConnection::Create(
    ServerConnectionContext&& context) {
  auto connection =
      std::shared_ptr<ServerConnection>(new ServerConnection(std::move(context)));
  connection->Start();
  return connection;
}

ServerConnection::ServerConnection(ServerConnectionContext&& context)
    : ServerConnectionContext{std::move(context)} {}

ServerConnection::~ServerConnection() {
  if (session_)
    session_->SetConnection(nullptr);
}

void ServerConnection::Start() {
  auto self = shared_from_this();
  boost::asio::co_spawn(
      transport_.get_executor(),
      [self]() -> transport::awaitable<void> {
        try {
          co_await self->Run();
        } catch (const std::exception& e) {
          LOG_ERROR(*self->logger_) << "Connection coroutine failed"
                                    << LOG_TAG("Error", e.what());
          self->Close();
        } catch (...) {
          LOG_ERROR(*self->logger_)
              << "Connection coroutine failed with unknown error";
          self->Close();
        }
      },
      boost::asio::detached);
}

transport::awaitable<void> ServerConnection::Run() {
  auto open_result = co_await transport_.open();
  if (open_result != transport::OK) {
    Close();
    OnTransportClosed(open_result);
    co_return;
  }

  auto cancelation = std::weak_ptr{cancelation_};
  std::vector<char> message;

  while (!cancelation.expired()) {
    // TODO: Revise buffer.
    message.resize(1024 * 1024);
    auto bytes_read = co_await transport_.read(message);

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

void ServerConnection::Shutdown() {
  closed_handler_ = {};
  Close();
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
  if (closed_)
    return;

  auto self = shared_from_this();
  closed_ = true;
  cancelation_.reset();

  if (session_) {
    session_->SetConnection(nullptr);
    session_ = nullptr;
  }

  if (closed_handler_) {
    closed_handler_(*this);
  }

  boost::asio::co_spawn(
      transport_.get_executor(),
      [self]() -> transport::awaitable<void> {
        co_await self->transport_.close();
      },
      boost::asio::detached);
}

void ServerConnection::OnTransportClosed(transport::error_code error) {
  Close();
}

void ServerConnection::Send(protocol::Message& message) {
  if (closed_)
    return;

  std::string string;
  if (!message.AppendToString(&string)) {
    throw std::runtime_error("Can't serialize the message");
  }

  auto self = shared_from_this();
  boost::asio::co_spawn(
      transport_.get_executor(),
      [self, string = std::move(string)]() -> transport::awaitable<void> {
        try {
          if (self->closed_)
            co_return;

          LOG_INFO(*self->logger_) << "Begin async write"
                                   << LOG_TAG("Size", string.size());
          auto bytes_written = co_await self->write_queue_.Write(string);
          LOG_INFO(*self->logger_) << "Async write completed"
                                   << LOG_TAG("Ok", bytes_written.ok())
                                   << LOG_TAG("BytesWritten",
                                              bytes_written.ok() ? *bytes_written
                                                                 : 0);
          if (!bytes_written.ok() || *bytes_written != string.size()) {
            self->Close();
            co_return;
          }
        } catch (const std::exception& e) {
          LOG_ERROR(*self->logger_) << "Async write failed"
                                    << LOG_TAG("Error", e.what());
          self->Close();
        } catch (...) {
          LOG_ERROR(*self->logger_) << "Async write failed with unknown error";
          self->Close();
        }
      },
      boost::asio::detached);
}

void ServerConnection::OnCreateSession(const protocol::Request& request) {
  assert(request.has_create_session());
  auto self = shared_from_this();
  boost::asio::co_spawn(
      transport_.get_executor(),
      [self, request]() -> Awaitable<void> {
        co_await self->OnCreateSessionAsync(request);
      },
      boost::asio::detached);
}

Awaitable<void> ServerConnection::OnCreateSessionAsync(protocol::Request request) {
  const auto request_id = request.request_id();
  auto result = co_await create_session_handler_(request.create_session());

  if (closed_)
    co_return;

  LOG_INFO(*logger_)
      << "CreateSession completed"
      << LOG_TAG("RequestId", request_id)
      << LOG_TAG("Status", ToString(result.status))
      << LOG_TAG("HasSession", result.session != nullptr);

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
  LOG_INFO(*logger_)
      << "Sending create-session response"
      << LOG_TAG("RequestId", request_id);
  Send(message);

  session_ = result.session;
  if (session_)
    LOG_INFO(*logger_)
        << "Binding session to connection"
        << LOG_TAG("RequestId", request_id);
  if (session_)
    session_->SetConnection(this);
}

void ServerConnection::OnDeleteSession(const protocol::Request& request) {
  assert(request.has_delete_session());
  auto self = shared_from_this();
  boost::asio::co_spawn(
      transport_.get_executor(),
      [self, request]() -> Awaitable<void> {
        co_await self->OnDeleteSessionAsync(request);
      },
      boost::asio::detached);
}

Awaitable<void> ServerConnection::OnDeleteSessionAsync(protocol::Request request) {
  scada::Status status(scada::StatusCode::Good);

  if (session_) {
    delete_session_handler_(*session_);
    session_ = nullptr;
  } else {
    status = scada::Status(scada::StatusCode::Bad_SessionIsLoggedOff);
  }

  if (!closed_) {
    protocol::Message message;
    auto& response = *message.add_responses();
    response.set_request_id(request.request_id());
    Convert(status, *response.mutable_status());

    try {
      Send(message);
    } catch (const std::exception&) {
    }
  }

  co_return;
}
