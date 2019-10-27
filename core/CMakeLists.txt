scada_module(scada_core)
target_link_libraries(scada_core PUBLIC scada_base)

if(WIN32)
  target_compile_options(scada_core PUBLIC "/std:c++latest")
  # Conformance mode
  target_compile_options(scada_core PRIVATE "/permissive-")
endif()
