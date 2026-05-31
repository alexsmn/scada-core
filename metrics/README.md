# OpenTelemetry Metrics

`otel_metrics.cpp` exports process metrics through OTLP/gRPC.

## Endpoint

By default, metrics are sent to:

```sh
localhost:4317
```

Override the endpoint with either environment variable:

```sh
export OTEL_EXPORTER_OTLP_METRICS_ENDPOINT=localhost:4317
```

or:

```sh
export OTEL_EXPORTER_OTLP_ENDPOINT=localhost:4317
```

`OTEL_EXPORTER_OTLP_METRICS_ENDPOINT` takes precedence. The exporter accepts
values with or without `http://` or `https://`; the scheme is stripped before
configuring the gRPC exporter.

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

DataCollector meters include `database_node_id` as a default attribute, so the
collector output can be filtered by historical database node ID.
