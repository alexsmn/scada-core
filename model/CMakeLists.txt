scada_module(scada_model)

target_link_libraries(scada_model PUBLIC scada_core)

# Modules-pilot consumer: node_id_util.cpp and namespaces.cpp import
# scada.core instead of including base/scada headers (see
# SCADA_USE_CORE_MODULE there).
scada_module_import_pilot(scada_model
  IMPORTS scada_core_module
  DEFINES SCADA_USE_CORE_MODULE)

scada_add_module_facade(scada_model FILES scada_model.cppm
  IMPORTS scada_core_module)
if(SCADA_CXX_MODULES)
  add_subdirectory(module_test)
endif()
