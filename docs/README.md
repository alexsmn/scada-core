# Core Docs

This directory documents the public-facing async service layer in `core/scada`
and the adapter types used to bridge between callback, promise, and coroutine
APIs.

Documents:

- [services.md](./services.md): core SCADA service APIs, async styles, and
  adapter reference

Related source headers:

- `core/scada/services.h`
- `core/scada/attribute_service.h`
- `core/scada/method_service.h`
- `core/scada/history_service.h`
- `core/scada/view_service.h`
- `core/scada/node_management_service.h`
- `core/scada/session_service.h`
- `core/scada/coroutine_services.h`
- `core/scada/*_service_promises.h`
- `core/scada/service_awaitable.h`
