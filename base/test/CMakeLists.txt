file(GLOB sources "*.cpp" "*.h")

add_library(base_unittest ${sources})

target_link_libraries(base_unittest PUBLIC
  base_test
  scada_base
  GTest::gmock
)

# Mitigate unreachable code in gmock-actions.h.
if(WIN32)
target_compile_options(base_unittest PUBLIC /wd4702)
endif()

set_property(TARGET base_unittest PROPERTY FOLDER ${scada_folder})
