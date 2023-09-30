# Protocol

add_library(scada_remote_protocol)

find_package(Protobuf REQUIRED)

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

target_sources(scada_remote_protocol PRIVATE
  ${SCADA_CORE_PROTO_CPP}
  ${SCADA_CORE_PROTO_H}
  ${SCADA_CORE_PROTO_GRPC_CPP}
  ${SCADA_CORE_PROTO_GRPC_H}
  ${SCADA_PROTO_CPP}
  ${SCADA_PROTO_H}
  ${SCADA_PROTO_GRPC_CPP}
  ${SCADA_PROTO_GRPC_H}
)

# For proto generated files
target_include_directories(scada_remote_protocol PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

target_link_libraries(scada_remote_protocol PUBLIC protobuf::libprotobuf)

if (gRPC_FOUND)
  target_link_libraries(scada_remote_protocol PUBLIC gRPC::grpc++)
endif()

if(WIN32)
  # Required for `SCADA_PROTO_CPP`:
  target_compile_options(scada_remote_protocol PRIVATE
    /wd4125
    /wd4127
    /wd4251
  )
endif()

# Conversion library

add_library(scada_remote_protocol_conversions)

target_sources(scada_remote_protocol_conversions PRIVATE
  "protocol_utils.cpp"
  "protocol_utils.h"
)

target_link_libraries(scada_remote_protocol_conversions PUBLIC
  scada_core
  scada_remote_protocol
)

if(WIN32)
  target_compile_options(scada_remote_protocol_conversions PRIVATE
    /wd4244 # 'conversion' conversion from 'type1' to 'type2', possible loss of data
  )
endif()

# Core library

scada_module(scada_core_remote)

target_link_libraries(scada_core_remote PUBLIC scada_core)

find_package(Net REQUIRED)

target_link_libraries(scada_core_remote PRIVATE
  scada_remote_protocol
  Net::net
)

set_property(TARGET scada_core_remote PROPERTY FOLDER ${scada_folder})

if(WIN32)
  target_compile_options(scada_core_remote PRIVATE
    /wd4244 # 'conversion' conversion from 'type1' to 'type2', possible loss of data
  )
endif()

# UTs

target_link_libraries(scada_core_remote_unittests PRIVATE
  scada_model
  scada_remote_protocol
  Net::net
)

set_property(TARGET scada_core_remote_unittests PROPERTY FOLDER ${scada_folder})
