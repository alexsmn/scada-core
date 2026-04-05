scada_module(scada_core)

target_precompile_headers(scada_core REUSE_FROM scada_base)

target_link_libraries(scada_core PUBLIC
  scada_base
  scada_metrics
)
