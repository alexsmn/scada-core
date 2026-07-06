// scada.remote — named C++20 module facade over the core/remote headers.
//
// Same design and rules as scada.base (see base/scada_base.cppm and
// docs/cxx-modules.md). `export import scada.core;` mirrors
// scada_core_remote's PUBLIC link on scada_core.
//
// Deliberately NOT in this facade: remote/protocol.h and
// remote/protocol_utils.h - they include the protobuf-generated scada.pb.h,
// and pb names would not be exported anyway. Keeping generated code out of
// the GMF avoids coupling the BMI to protobuf codegen outputs and shrinks
// the pcm. TUs that need the Convert() overloads, PROTOCOL_VERSION_*, or pb
// message types keep including those headers textually (before the import).
// The protocol::Message forward declarations in the proxy/stub headers are
// harmless (not exported).

module;

// ---- Global module fragment: headers stay the source of truth ----
#include "remote/connection.h"
#include "remote/history_proxy.h"
#include "remote/history_stub.h"
#include "remote/message_sender.h"
#include "remote/monitored_item_proxy.h"
#include "remote/monitored_item_router.h"
#include "remote/node_management_proxy.h"
#include "remote/node_management_stub.h"
#include "remote/protocol_buffer.h"
#include "remote/protocol_message_reader.h"
#include "remote/protocol_message_transport.h"
#include "remote/remote_connection.h"
#include "remote/remote_listener.h"
#include "remote/remote_services.h"
#include "remote/remote_session_manager.h"
#include "remote/session_proxy.h"
#include "remote/session_proxy_debuger.h"
#include "remote/session_proxy_notifier.h"
#include "remote/session_stub.h"
#include "remote/subscription.h"
#include "remote/subscription_proxy.h"
#include "remote/subscription_stub.h"
#include "remote/view_event_queue.h"
#include "remote/view_service_proxy.h"
#include "remote/view_service_stub.h"

export module scada.remote;

export import scada.core;

export {
  // Proxy/stub/session types (global namespace).
  using ::HistoryProxy;
  using ::HistoryStub;
  using ::MonitoredItemProxy;
  using ::MonitoredItemRouter;
  using ::NodeManagementProxy;
  using ::NodeManagementStub;
  using ::ProtocolMessageReader;
  using ::ProtocolMessageTransport;
  using ::RemoteListener;
  using ::RemoteSessionManager;
  using ::RemoteSessionManagerContext;
  using ::SessionProxy;
  using ::SessionProxyContext;
  using ::SessionProxyDebugger;
  using ::SessionProxyNotifier;
  using ::SessionStub;
  using ::SubscriptionProxy;
  using ::SubscriptionStub;
  using ::ViewEventQueue;
  using ::ViewServiceProxy;
  using ::ViewServiceStub;
  using ::ViewServiceStubContext;

  // remote_connection.h / session_stub.h
  using ::CreateSessionResult;
  using ::ServerConnection;
  using ::ServerConnectionContext;
  using ::SessionContext;

  // connection.h / message_sender.h (global classes; only the
  // protocol::Message forward declaration lives in namespace protocol)
  using ::Connection;
  using ::MessageSender;

  // subscription.h
  using ::MonitoredItemParams;
  using ::SubscriptionParams;

  // remote_services.h
  using ::CreateRemoteServices;
}  // export

export namespace protocol {

// protocol_buffer.h (the inline framing helpers; kHeaderSize is a
// namespace-scope constexpr => internal linkage, include-only)
using protocol::AppendMessage;
using protocol::GetIncomingMessageSize;
using protocol::GetMessagePayload;
using protocol::GetMessagePayloadSize;
using protocol::MessageSizeType;
using protocol::PrependMessageSize;
using protocol::UpdateMessageSize;

}  // namespace protocol
