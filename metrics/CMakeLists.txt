scada_module(scada_metrics)

target_precompile_headers(scada_metrics REUSE_FROM scada_base)

target_link_libraries(scada_metrics PUBLIC scada_base)

if(WIN32)
  # Required by Boost.Uuid.
  target_link_libraries(scada_metrics PRIVATE Bcrypt)
endif()
