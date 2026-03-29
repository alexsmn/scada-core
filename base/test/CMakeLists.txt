file(GLOB sources "*.cpp" "*.h")

add_library(base_unittest ${sources})

find_package(GTest REQUIRED)

target_link_libraries(base_unittest PUBLIC
  scada_base
  GTest::gmock
)

# Mitigate unreachable code in gmock-actions.h.
if(WIN32)
target_compile_options(base_unittest PUBLIC /wd4702)
endif()

set_property(TARGET base_unittest PROPERTY FOLDER ${scada_core_folder})