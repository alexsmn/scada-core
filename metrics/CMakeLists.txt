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
  opentelemetry-cpp::in_memory_metric_exporter
  opentelemetry-cpp::otlp_grpc_metrics_exporter
)

if(WIN32)
  # Required by Boost.Uuid.
  target_link_libraries(scada_metrics PRIVATE Bcrypt)
endif()

if(SCADA_CXX_MODULES)
  # Modules-pilot consumer: meter.cpp imports scada.base instead of including
  # base headers (see SCADA_USE_BASE_MODULE there). Scanning is opt-in
  # because the tree's policy version predates CMP0155; the launcher is
  # cleared because ccache does not hash the imported BMI and can restore
  # stale objects for importing TUs (see base/CMakeLists.txt).
  target_link_libraries(scada_metrics PRIVATE scada_base_module)
  target_compile_definitions(scada_metrics PRIVATE SCADA_USE_BASE_MODULE)
  set_target_properties(scada_metrics PROPERTIES
    CXX_SCAN_FOR_MODULES ON
    CXX_COMPILER_LAUNCHER "")
endif()
