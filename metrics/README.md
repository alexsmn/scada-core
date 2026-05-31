# OpenTelemetry Metrics

`otel_metrics.cpp` exports process metrics through OTLP/gRPC.

## Endpoint

The endpoint is provided by the process runtime through
`OpenTelemetryMetricsOptions::endpoint`. The exporter accepts values with or
without `http://` or `https://`; the scheme is stripped before configuring the
gRPC exporter.

For local development, use:

```sh
localhost:4317
```

## View Metrics Locally

Run an OpenTelemetry Collector with a debug exporter:

```yaml
receivers:
  otlp:
    protocols:
      grpc:
        endpoint: 0.0.0.0:4317

exporters:
  debug:
    verbosity: detailed

service:
  pipelines:
    metrics:
      receivers: [otlp]
      exporters: [debug]
```

Save this as `otel-collector.yaml`, then run:

```sh
docker run --rm \
  -p 4317:4317 \
  -v "$PWD/otel-collector.yaml:/etc/otelcol/config.yaml" \
  otel/opentelemetry-collector:latest
```

Start the SCADA server or client in another terminal. Reported metrics will be
printed in the collector output. The `service.name` resource attribute is set by
the process runtime, for example `scada-server` or `scada-client`.

## Useful Metrics

History collection currently reports metrics such as:

```text
scada.history.data_collector.item_count
scada.history.data_collector.pending_task_count
scada.history.data_collector.write_value_count
scada.history.data_collector.write_value_duration_us
```

DataCollector meters include `node_id` as a default attribute, so the
collector output can be filtered by historical database node ID.
