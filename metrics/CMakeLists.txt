scada_module(scada_metrics)

find_package(opentelemetry-cpp CONFIG REQUIRED)

target_precompile_headers(scada_metrics REUSE_FROM scada_base)

target_link_libraries(scada_metrics PUBLIC
  scada_base
  opentelemetry-cpp::api
  opentelemetry-cpp::sdk
  opentelemetry-cpp::metrics
  opentelemetry-cpp::otlp_grpc_metrics_exporter
)

if(WIN32)
  # Required by Boost.Uuid.
  target_link_libraries(scada_metrics PRIVATE Bcrypt)
endif()
