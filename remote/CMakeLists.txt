find_package(Protobuf REQUIRED)

file(GLOB SOURCES CONFIGURE_DEPENDS "*.cpp" "*.h" "*.proto")
file(GLOB UT_SOURCES CONFIGURE_DEPENDS "*_unittest.*")

list(REMOVE_ITEM SOURCES ${UT_SOURCES})

protobuf_generate_cpp(SCADA_CORE_PROTO_CPP SCADA_CORE_PROTO_H scada_core.proto)
protobuf_generate_cpp(SCADA_PROTO_CPP SCADA_PROTO_H scada.proto)
protobuf_generate_python(SCADA_PROTO_PY scada.proto)

# find_package(gRPC)
if (gRPC_FOUND)
  # get_target_property(GRPC_CPP_PLUGIN grpc::grpc_cpp_plugin IMPORTED_LOCATION)
  include(protobuf_grpc.cmake)
  protobuf_generate_grpc_cpp(SCADA_CORE_PROTO_GRPC_CPP SCADA_CORE_PROTO_GRPC_H scada_core.proto)
  protobuf_generate_grpc_cpp(SCADA_PROTO_GRPC_CPP SCADA_PROTO_GRPC_H scada.proto)
endif()

list(APPEND SOURCES
  ${SCADA_CORE_PROTO_CPP}
  ${SCADA_CORE_PROTO_H}
  ${SCADA_CORE_PROTO_GRPC_CPP}
  ${SCADA_CORE_PROTO_GRPC_H}
  ${SCADA_PROTO_CPP}
  ${SCADA_PROTO_H}
  ${SCADA_PROTO_GRPC_CPP}
  ${SCADA_PROTO_GRPC_H}
)

add_library(scada_core_remote ${SOURCES})

# For proto generated files
target_include_directories(scada_core_remote PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

target_link_libraries(scada_core_remote PUBLIC
  protobuf::libprotobuf
  net
  scada_core
  address_space
)

if (gRPC_FOUND)
  target_link_libraries(scada_core_remote PUBLIC gRPC::grpc++)
endif()

# Required by gRPC.
# target_compile_definitions(scada_core_remote PUBLIC
#   _SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING
# )

set_property(TARGET scada_core_remote PROPERTY FOLDER ${scada_folder})

if(WIN32)
  target_compile_options(scada_core_remote PRIVATE
    /wd4244 # 'conversion' conversion from 'type1' to 'type2', possible loss of data
  )

  # Required for `SCADA_PROTO_CPP`:
  target_compile_options(scada_core_remote PRIVATE
    /wd4125
    /wd4127
    /wd4251
  )
endif()

# UTs

add_executable(scada_core_remote_unittests ${UT_SOURCES})
add_test(NAME scada_core_remote_unittests COMMAND scada_core_remote_unittests)
target_link_libraries(scada_core_remote_unittests PUBLIC
  scada_core_remote
  base_unittest
)

set_property(TARGET scada_core_remote_unittests PROPERTY FOLDER ${scada_folder})
