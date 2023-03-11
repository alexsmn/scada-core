include(GoogleTest)

if(1)

find_package(GTest REQUIRED)

# get_target_property(LIBA_INCLUDES LibraryA INCLUDE_DIRECTORIES)

# set(GTEST_INCLUDE_DIRS "${GTest_DIR}/googletest/googletest/include")
# set(GMOCK_INCLUDE_DIRS "${GTest_DIR}/googletest/googlemock/include")

else()

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
set(GTEST_FOUND TRUE CACHE BOOL "" FORCE)

add_subdirectory("${deps}/googletest" gtest EXCLUDE_FROM_ALL)

target_compile_definitions(gtest PUBLIC
  -DGTEST_HAS_TR1_TUPLE=0
  -DGTEST_USE_OWN_TR1_TUPLE=0
  -DGTEST_LANG_CXX11=1
)

set(GTEST_INCLUDE_DIRS "${deps}/googletest/googletest/include")
set(GMOCK_INCLUDE_DIRS "${deps}/googletest/googlemock/include")

endif()