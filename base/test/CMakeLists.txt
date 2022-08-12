file(GLOB sources "*.cpp" "*.h")

add_library(base_unittest ${sources})

target_link_libraries(base_unittest PUBLIC
  base_test
  scada_base
  GTest::gmock
)

# Mitigate unrechable code in gmock-actions.h.
target_compile_options(base_unittest PUBLIC /wd4702)

set_property(TARGET base_unittest PROPERTY FOLDER ${scada_folder})
