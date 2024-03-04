scada_module(scada_metrics)

target_link_libraries(scada_metrics PUBLIC scada_base)

if(WIN32)
  # Required by Boost.Uuid.
  target_link_libraries(scada_metrics PRIVATE Bcrypt)
endif()
