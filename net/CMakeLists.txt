scada_module(scada_net)

find_package(Transport REQUIRED)

target_link_libraries(scada_net PUBLIC scada_base transport)

scada_add_module_facade(scada_net FILES scada_net.cppm
  IMPORTS scada_base_module)
if(SCADA_CXX_MODULES)
  add_subdirectory(module_test)
endif()
