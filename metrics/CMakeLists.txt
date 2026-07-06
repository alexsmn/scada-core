scada_module(scada_metrics)

find_package(opentelemetry-cpp CONFIG REQUIRED)

# No PCH in the modules pilot: clang-scan-deps cannot read the AppleClang
# PCH and silently emits an empty dependency scan (the import goes
# unnoticed), then the -MT/-MF form fails outright.
if(NOT SCADA_CXX_MODULES)
  target_precompile_headers(scada_metrics REUSE_FROM scada_base)
endif()

target_link_libraries(scada_metrics PUBLIC
  scada_base
  opentelemetry-cpp::api
  opentelemetry-cpp::sdk
  opentelemetry-cpp::metrics
  opentelemetry-cpp::trace
  opentelemetry-cpp::in_memory_metric_exporter
  opentelemetry-cpp::in_memory_span_exporter
  opentelemetry-cpp::otlp_grpc_metrics_exporter
  opentelemetry-cpp::otlp_grpc_exporter
)

if(WIN32)
  # Required by Boost.Uuid.
  target_link_libraries(scada_metrics PRIVATE Bcrypt)
endif()

# Modules-pilot consumer: meter.cpp imports scada.base instead of including
# base headers (see SCADA_USE_BASE_MODULE there).
scada_module_import_pilot(scada_metrics
  IMPORTS scada_base_module
  DEFINES SCADA_USE_BASE_MODULE)

scada_add_module_facade(scada_metrics FILES scada_metrics.cppm
  IMPORTS scada_base_module)
if(SCADA_CXX_MODULES)
  add_subdirectory(module_test)
endif()
