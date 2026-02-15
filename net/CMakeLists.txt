scada_module(scada_net)

find_package(Transport REQUIRED)

target_link_libraries(scada_net PUBLIC scada_base transport)
