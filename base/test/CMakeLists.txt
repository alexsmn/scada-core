file(GLOB sources "*.cpp" "*.h")

add_library(base_unittest ${sources})

target_link_libraries(base_unittest PUBLIC
  base_test
  scada_base
  gmock_main
)
set_property(TARGET base_unittest PROPERTY FOLDER ${scada_folder})
