scada_module(scada_core)

# No PCH in the modules pilot: clang-scan-deps cannot read the AppleClang
# PCH and silently emits an empty dependency scan (see docs/cxx-modules.md).
if(NOT SCADA_CXX_MODULES)
  target_precompile_headers(scada_core REUSE_FROM scada_base)
endif()

target_link_libraries(scada_core PUBLIC
  scada_base
  scada_metrics
)

# Modules-pilot consumer: status.cpp and node_id.cpp import scada.base
# instead of including base headers (see SCADA_USE_BASE_MODULE there).
scada_module_import_pilot(scada_core
  IMPORTS scada_base_module
  DEFINES SCADA_USE_BASE_MODULE)

scada_add_module_facade(scada_core FILES scada_core.cppm
  IMPORTS scada_base_module scada_metrics_module)
if(SCADA_CXX_MODULES)
  add_subdirectory(module_test)
endif()
