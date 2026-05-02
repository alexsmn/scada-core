# Core Service APIs And Adapters

## Summary

`core/scada` exposes the system's main service contracts through
`scada::services` in `core/scada/services.h`.

The service set is:

- `AttributeService`
- `MonitoredItemService`
- `MethodService`
- `HistoryService`
- `ViewService`
- `NodeManagementService`
- `SessionService`

These services are intentionally split across two async styles today:

- callback-first service interfaces for most request/response services
- coroutine-first session APIs, free helpers, and adapter interfaces

The new coroutine layer in `core/scada/coroutine_services.h` standardizes a
coroutine-facing shape for the async core services without breaking the
existing callback contracts.

## Service Locator

`scada::services` is a plain aggregate of raw pointers:

```cpp
struct services {
  AttributeService* attribute_service = nullptr;
  MonitoredItemService* monitored_item_service = nullptr;
  MethodService* method_service = nullptr;
  HistoryService* history_service = nullptr;
  ViewService* view_service = nullptr;
  NodeManagementService* node_management_service = nullptr;
  SessionService* session_service = nullptr;
};
```

Use it as a wiring boundary between modules. It does not own the pointed-to
services.

`DataServices` in `core/scada/data_services.h` provides shared/unowned wrapper
helpers when a service bundle must cross ownership boundaries.

## Async Styles

### Callback APIs

Most SCADA services are callback-based:

- `AttributeService`
- `MethodService`
- `HistoryService`
- `ViewService`
- `NodeManagementService`

These interfaces usually report one of two result shapes:

- `Status + vector<...>` for batched service calls
- result structs carrying their own `Status` for history reads

Examples:

```cpp
service.Read(context, inputs,
             [](Status status, std::vector<DataValue> results) {
               // Handle status and result batch.
             });

auto status = co_await service.Call(node_id, method_id, arguments, user_id);
```

### Coroutine APIs

Before the adapter layer, `core/scada/service_awaitable.h` exposed free
awaitable wrappers such as:

- `ReadAsync`
- `WriteAsync`
- `CallAsync`
- `HistoryReadRawAsync`
- `BrowseAsync`
- `AddNodesAsync`

Those helpers remain useful for direct adaptation of existing callback
services. They now centralize both executor entrypoints on one implementation
path: the legacy `std::shared_ptr<Executor>` overloads forward to the
`AnyExecutor` overloads, so coroutine consumers in `common/` and `server/`
reuse the same callback-to-awaitable bridge instead of maintaining local
copies.

`core/scada/coroutine_services.h` adds coroutine-native service interfaces and
named adapters so coroutine-based implementations can participate in the same
service graph.

## Core Service APIs

### AttributeService

Header: `core/scada/attribute_service.h`

Purpose:

- read node attributes in batches
- write node attributes in batches

Primary API:

```cpp
virtual void Read(
    const ServiceContext& context,
    const std::shared_ptr<const std::vector<ReadValueId>>& inputs,
    const ReadCallback& callback) = 0;

virtual void Write(
    const ServiceContext& context,
    const std::shared_ptr<const std::vector<WriteValue>>& inputs,
    const WriteCallback& callback) = 0;
```

Notes:

- batching is part of the contract
- single-item inline helpers `Read(...)` and `Write(...)` are also provided
- `WriteCallback` is `MultiStatusCallback`

Coroutine equivalent:

```cpp
class CoroutineAttributeService {
  virtual Awaitable<StatusOr<std::vector<DataValue>>> Read(
      ServiceContext context,
      std::shared_ptr<const std::vector<ReadValueId>> inputs) = 0;

  virtual Awaitable<StatusOr<std::vector<StatusCode>>> Write(
      ServiceContext context,
      std::shared_ptr<const std::vector<WriteValue>> inputs) = 0;
};
```

### MethodService

Header: `core/scada/method_service.h`

Purpose:

- invoke a method on a target node as a specific user

Primary API:

```cpp
virtual void Call(const NodeId& node_id,
                  const NodeId& method_id,
                  const std::vector<Variant>& arguments,
                  const NodeId& user_id,
                  const StatusCallback& callback) = 0;
```

Coroutine equivalent:

```cpp
class MethodService {
  virtual Awaitable<Status> Call(NodeId node_id,
                                 NodeId method_id,
                                 std::vector<Variant> arguments,
                                 NodeId user_id) = 0;
};
```

### HistoryService

Header: `core/scada/history_service.h`

Purpose:

- read raw historical values
- read historical events

Primary API:

```cpp
class HistoryService {
  virtual Awaitable<HistoryReadRawResult> HistoryReadRaw(
      HistoryReadRawDetails details) = 0;

  virtual Awaitable<HistoryReadEventsResult> HistoryReadEvents(
      NodeId node_id,
      base::Time from,
      base::Time to,
      EventFilter filter) = 0;
};
```

History result structs carry the `Status` inside the result object.

### ViewService

Header: `core/scada/view_service.h`

Purpose:

- browse references from one or more nodes
- translate browse paths into target nodes

Primary API:

```cpp
virtual void Browse(const ServiceContext& context,
                    const std::vector<BrowseDescription>& inputs,
                    const BrowseCallback& callback) = 0;

virtual void TranslateBrowsePaths(
    const std::vector<BrowsePath>& inputs,
    const TranslateBrowsePathsCallback& callback) = 0;
```

Coroutine equivalent:

```cpp
class ViewService {
  virtual Awaitable<StatusOr<std::vector<BrowseResult>>> Browse(
      ServiceContext context,
      std::vector<BrowseDescription> inputs) = 0;

  virtual Awaitable<StatusOr<std::vector<BrowsePathResult>>>
  TranslateBrowsePaths(std::vector<BrowsePath> inputs) = 0;
};
```

### NodeManagementService

Header: `core/scada/node_management_service.h`

Purpose:

- add nodes
- delete nodes
- add references
- delete references

API:

```cpp
class NodeManagementService {
  virtual Awaitable<StatusOr<std::vector<AddNodesResult>>> AddNodes(
      std::vector<AddNodesItem> inputs) = 0;

  virtual Awaitable<StatusOr<std::vector<StatusCode>>> DeleteNodes(
      std::vector<DeleteNodesItem> inputs) = 0;

  virtual Awaitable<StatusOr<std::vector<StatusCode>>>
  AddReferences(std::vector<AddReferencesItem> inputs) = 0;

  virtual Awaitable<StatusOr<std::vector<StatusCode>>>
  DeleteReferences(std::vector<DeleteReferencesItem> inputs) = 0;
};
```

### SessionService

Header: `core/scada/session_service.h`

Purpose:

- connect, reconnect, and disconnect sessions
- expose session identity and privilege state
- publish session-state-change notifications

Primary API:

```cpp
virtual Awaitable<void> Connect(const SessionConnectParams& params) = 0;
virtual Awaitable<void> Reconnect() = 0;
virtual Awaitable<void> Disconnect() = 0;

virtual bool IsConnected(base::TimeDelta* ping_delay = nullptr) const = 0;
virtual NodeId GetUserId() const = 0;
virtual bool HasPrivilege(Privilege privilege) const = 0;
virtual std::string GetHostName() const = 0;
virtual bool IsScada() const = 0;
```

Session lifecycle methods are coroutine-native. State/query methods remain
synchronous.

### MonitoredItemService

Header: `core/scada/monitored_item_service.h`

Purpose:

- create a subscription object for value monitoring

API:

```cpp
virtual std::shared_ptr<MonitoredItem> CreateMonitoredItem(
    const ReadValueId& value_id,
    const MonitoringParameters& params) = 0;
```

This service is not part of the new coroutine adapter layer because its public
contract is already object/lifetime-oriented rather than callback-based.

## Adapter Reference

Header: `core/scada/coroutine_services.h`

### Callback To Coroutine

Use these when the implementation you already have is callback-based, but the
consumer wants an awaitable service interface.

Adapters:

- `CallbackToCoroutineAttributeServiceAdapter`

Construction pattern:

```cpp
auto executor = std::shared_ptr<Executor>{...};
CallbackToCoroutineAttributeServiceAdapter adapter{executor, callback_service};
```

Behavior:

- uses `CallbackToAwaitable(...)`
- resumes the awaiting coroutine on the provided executor
- preserves the original callback service as the source of truth

### Coroutine To Callback

Use these when the implementation is coroutine-first, but existing call sites
or service registries still expect callback interfaces.

Adapters:

- `CoroutineToCallbackAttributeServiceAdapter`
- `HistoryServiceAdapter_REMOVED`

Behavior:

- uses `CoSpawn(...)` on the provided executor
- invokes the existing callback contract when the coroutine completes
- translates thrown exceptions into bad-status callback results

Exception mapping rules:

- generic exceptions map through `GetExceptionStatus(...)`
- `status_exception` preserves its embedded `Status`
- callback forms that return batched results return an empty result vector on
  adapter-level failure
- history result adapters return a result object with bad `status`

## Choosing An API Style

Recommended default for new internal async logic:

1. implement new async workflows as coroutines
2. adapt attribute callbacks at module boundaries where required
3. keep `scada::services` wiring stable until the caller graph is migrated

Practical rules:

- use callback services only for service interfaces that still expose callbacks
- use coroutine services for new orchestration code or migrations away from
  nested callbacks
- prefer named adapters from `coroutine_services.h` over ad hoc lambda bridges

## Minimal Examples

### Awaiting A Callback Service Through An Adapter

```cpp
CallbackToCoroutineAttributeServiceAdapter async_attribute_service{
    executor, attribute_service};

auto [status, values] =
    co_await async_attribute_service.Read(context, read_inputs);
```

### Exposing A Coroutine Attribute Service As A Legacy Callback Service

```cpp
CoroutineToCallbackAttributeServiceAdapter attribute_adapter{executor,
                                                            coroutine_attribute};

attribute_adapter.Read(context, inputs,
                       [](Status status, std::vector<DataValue> results) {
                         // Legacy callback consumer.
                       });
```

## Tests

Adapter behavior is covered by:

- `core/scada/coroutine_services_unittest.cpp`

The unit tests validate:

- request argument forwarding
- result forwarding
- callback-to-coroutine bridging
- coroutine-to-callback bridging
- coroutine exception to status conversion
- coroutine session forwarding
