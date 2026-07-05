#pragma once

// The OpenTelemetry SpanKind subset used by SCADA spans: kServer for handling
// an inbound request, kClient for issuing an outbound (inter-tier) request,
// kInternal for everything else.
enum class TraceSpanKind { kInternal, kServer, kClient };
