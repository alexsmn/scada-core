# ProtoBuf
if(NOT PROTOBUF_FOUND)
  find_package(Protobuf REQUIRED)
endif()

scada_module(scada_core_remote)

protobuf_generate_cpp(scada_proto_cpp scada_proto_h scada.proto)
protobuf_generate_python(scada_proto_py scada.proto)

target_sources(scada_core_remote PRIVATE ${scada_proto_cpp} ${scada_proto_h})

# For proto generated files
target_include_directories(scada_core_remote PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

target_link_libraries(scada_core_remote PUBLIC
  protobuf::libprotobuf
  net
  scada_core
  scada_model
)