#include "remote/remote_connection.h"

#include "base/check.h"
#include "remote/protocol.h"
#include "remote/protocol_buffer.h"
#include "remote/protocol_utils.h"
#include "remote/session_stub.h"

#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <chrono>

namespace {

// A connection is closed when nothing has arrived on it for this long.
//
// The peer pings once a second (`kPingDelay` in session_proxy.cpp) for as long
// as its session lives, so this is sixty missed pings — a wide margin against a
// merely busy, or briefly suspended, client.
//
// The deadline exists for the disconnect the socket never reports. An ordinary
// disconnect (process exit, kill, close) errors the pending read and
// `ServerConnection::Run` closes the connection, which releases the session.
// A peer host that loses power, a network partition, or a NAT that silently
// drops the flow leaves an established connection whose read simply never
// completes — and without a deadline that connection, and the session it holds,
// outlive the peer for the life of the process. For a single-session account
// that is a permanent lockout, because the logon gate counts any session a
// connection is still serving.
constexpr auto kIdleTimeout = std::chrono::seconds{60};

}  // namespace

std::shared_ptr<ServerConnection> ServerConnection::Create(
    ServerConnectionContext&& context) {
  auto connection = std::shared_ptr<ServerConnection>(
      new ServerConnection(std::move(context)));
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
  ArmIdleTimer();

  auto self = shared_from_this();
  boost::asio::co_spawn(
      transport_.get_executor(),
      [self]() -> transport::awaitable<void> {
        try {
          co_await self->Run();
        } catch (const std::exception& e) {
          LOG_ERROR(*self->logger_)
              << "Connection coroutine failed" << LOG_TAG("Error", e.what());
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
  // The framed Scada protocol has no chunking, so the buffer must fit the
  // largest single message the peer sends (see protocol::kMaxMessageSize).
  // Size it once, outside the loop, and hand each message out as a subspan —
  // resizing per iteration burned a 16 MiB value-initialization plus a 16 MiB
  // destroy on every read. See the matching note in SessionProxy::Connect.
  std::vector<char> message(protocol::kMaxMessageSize);

  while (!cancelation.expired()) {
    auto bytes_read = co_await transport_.read(message);

    if (cancelation.expired()) {
      co_return;
    }

    if (!bytes_read.ok() || *bytes_read == 0) {
      Close();
      co_return;
    }

    OnTransportMessageReceived(std::span{message}.first(*bytes_read));
  }
}

void ServerConnection::OnSessionDeleted() {
  session_ = nullptr;
}

void ServerConnection::Shutdown() {
  closed_handler_ = {};
  Close();
}

void ServerConnection::ArmIdleTimer() {
  if (closed_) {
    return;
  }

  if (!idle_timer_) {
    idle_timer_.emplace(transport_.get_executor());
  }

  // Rescheduling cancels the armed wait; its handler runs with
  // `operation_aborted` and returns without touching anything.
  idle_timer_->expires_after(kIdleTimeout);
  idle_timer_->async_wait(
      [weak_self = weak_from_this()](boost::system::error_code error) {
        if (error == boost::asio::error::operation_aborted) {
          return;
        }
        if (auto self = weak_self.lock()) {
          self->OnIdleTimeout();
        }
      });
}

void ServerConnection::OnIdleTimeout() {
  if (closed_) {
    return;
  }

  LOG_WARNING(*logger_)
      << "Closing idle connection"
      << LOG_TAG("IdleSeconds",
                 std::chrono::duration_cast<std::chrono::seconds>(kIdleTimeout)
                     .count())
      << LOG_TAG("HasSession", session_ != nullptr);

  Close();
}

void ServerConnection::OnTransportMessageReceived(std::span<const char> data) {
  ArmIdleTimer();

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
  idle_timer_.reset();

  if (session_) {
    auto* session = session_;
    session_ = nullptr;
    session->SetConnection(nullptr);

    if (delete_session_handler_) {
      delete_session_handler_(*session);
    }
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
    LOG_ERROR(*logger_) << "Cannot serialize protocol message";
    Close();
    return;
  }

  auto self = shared_from_this();
  boost::asio::co_spawn(
      transport_.get_executor(),
      [self, string = std::move(string)]() -> transport::awaitable<void> {
        try {
          if (self->closed_)
            co_return;

          LOG_DEBUG(*self->logger_)
              << "Begin async write" << LOG_TAG("Size", string.size());
          auto bytes_written = co_await self->write_queue_.Write(string);
          LOG_DEBUG(*self->logger_)
              << "Async write completed" << LOG_TAG("Ok", bytes_written.ok())
              << LOG_TAG("BytesWritten",
                         bytes_written.ok() ? *bytes_written : 0);
          if (!bytes_written.ok() || *bytes_written != string.size()) {
            self->Close();
            co_return;
          }
        } catch (const std::exception& e) {
          LOG_ERROR(*self->logger_)
              << "Async write failed" << LOG_TAG("Error", e.what());
          self->Close();
        } catch (...) {
          LOG_ERROR(*self->logger_) << "Async write failed with unknown error";
          self->Close();
        }
      },
      boost::asio::detached);
}

void ServerConnection::OnCreateSession(const protocol::Request& request) {
  scada::base::Check(request.has_create_session());
  auto self = shared_from_this();
  boost::asio::co_spawn(
      transport_.get_executor(),
      [self, request]() -> Awaitable<void> {
        co_await self->OnCreateSessionAsync(request);
      },
      boost::asio::detached);
}

Awaitable<void> ServerConnection::OnCreateSessionAsync(
    protocol::Request request) {
  const auto request_id = request.request_id();
  auto result = co_await create_session_handler_(request.create_session());

  if (closed_) {
    // The peer went away while authentication was in flight. The session now
    // exists in the manager, but this connection never bound it — so the
    // Close() that ran during the await saw no session and could not release
    // it. Release it here: otherwise it stays in the session map for the life
    // of the process, and for a single-session account every later logon is
    // refused with Bad_UserIsAlreadyLoggedOn by a session nothing can reach.
    if (result.session && delete_session_handler_) {
      LOG_INFO(*logger_) << "Releasing session created after connection closed"
                         << LOG_TAG("RequestId", request_id);
      delete_session_handler_(*result.session);
    }
    co_return;
  }

  LOG_INFO(*logger_) << "CreateSession completed"
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
  LOG_INFO(*logger_) << "Sending create-session response"
                     << LOG_TAG("RequestId", request_id);
  Send(message);

  session_ = result.session;
  if (session_)
    LOG_INFO(*logger_) << "Binding session to connection"
                       << LOG_TAG("RequestId", request_id);
  if (session_)
    session_->SetConnection(this);
}

void ServerConnection::OnDeleteSession(const protocol::Request& request) {
  scada::base::Check(request.has_delete_session());
  auto self = shared_from_this();
  boost::asio::co_spawn(
      transport_.get_executor(),
      [self, request]() -> Awaitable<void> {
        co_await self->OnDeleteSessionAsync(request);
      },
      boost::asio::detached);
}

Awaitable<void> ServerConnection::OnDeleteSessionAsync(
    protocol::Request request) {
  scada::Status status(scada::StatusCode::Good);

  if (session_) {
    auto* session = session_;
    session_ = nullptr;
    // Drop the session's raw back-pointer before notifying higher layers.
    // Late service completions can still arrive after delete-session and must
    // observe a null connection instead of a dangling ServerConnection*.
    session->SetConnection(nullptr);
    delete_session_handler_(*session);
  } else {
    status = scada::Status(scada::StatusCode::Bad_SessionIsLoggedOff);
  }

  if (!closed_) {
    protocol::Message message;
    auto& response = *message.add_responses();
    response.set_request_id(request.request_id());
    Convert(status, *response.mutable_status());

    Send(message);
  }

  co_return;
}
