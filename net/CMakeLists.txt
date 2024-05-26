scada_module(scada_net)

find_package(Net REQUIRED)

target_link_libraries(scada_net PUBLIC scada_base net)
