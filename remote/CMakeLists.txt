# ProtoBuf
if(NOT PROTOBUF_FOUND)
  find_package(Protobuf REQUIRED)
endif()

file(GLOB sources CONFIGURE_DEPENDS "*.cpp" "*.h" "*.proto")
file(GLOB unittests CONFIGURE_DEPENDS "*_unittest.*")

list(REMOVE_ITEM sources ${unittests})

protobuf_generate_cpp(scada_proto_cpp scada_proto_h scada.proto)
protobuf_generate_python(scada_proto_py scada.proto)

list(APPEND sources ${scada_proto_cpp} ${scada_proto_h})

add_library(scada_core_remote ${sources})

# For proto generated files
target_include_directories(scada_core_remote PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

target_link_libraries(scada_core_remote PUBLIC
  protobuf::libprotobuf
  net
  scada_core
  address_space
)

set_property(TARGET scada_core_remote PROPERTY FOLDER ${scada_folder})

if(WIN32)
  target_compile_options(scada_core_remote PRIVATE
    /wd4244 # 'conversion' conversion from 'type1' to 'type2', possible loss of data
  )

  # Required for `scada_proto_cpp`:
  target_compile_options(scada_core_remote PRIVATE
    /wd4125
    /wd4127
    /wd4251
  )
endif()

# UTs

add_executable(scada_core_remote_unittests ${unittests})
add_test(NAME scada_core_remote_unittests COMMAND scada_core_remote_unittests)
target_link_libraries(scada_core_remote_unittests PUBLIC
  scada_core_remote
  gmock_main
)

set_property(TARGET scada_core_remote_unittests PROPERTY FOLDER ${scada_folder})
