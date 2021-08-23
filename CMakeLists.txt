include(boost.cmake)
include(gtest.cmake)
include(scada_module.cmake)

add_subdirectory("${deps}/icu" icu)

add_subdirectory("${deps}/net" net)

add_subdirectory(base)
add_subdirectory(core)
add_subdirectory(model)
add_subdirectory(remote)

if(WIN32)
  include(vc-redist.cmake)

  install(FILES
      "$<$<CONFIG:Debug>:${LIBRARY_OUTPUT_PATH}/Debug/libprotobufd.dll>"
      "$<$<CONFIG:RelWithDebInfo>:${LIBRARY_OUTPUT_PATH}/RelWithDebInfo/libprotobuf.dll>"
      "$<$<CONFIG:Debug>:${LIBRARY_OUTPUT_PATH}/Debug/OPCUA.dll>"
      "$<$<CONFIG:RelWithDebInfo>:${LIBRARY_OUTPUT_PATH}/RelWithDebInfo/OPCUA.dll>"
      "$<$<CONFIG:Debug>:${LIBRARY_OUTPUT_PATH}/Debug/libssl-1_1.dll>"
      "$<$<CONFIG:RelWithDebInfo>:${LIBRARY_OUTPUT_PATH}/RelWithDebInfo/libssl-1_1.dll>"
      "$<$<CONFIG:Debug>:${LIBRARY_OUTPUT_PATH}/Debug/libcrypto-1_1.dll>"
      "$<$<CONFIG:RelWithDebInfo>:${LIBRARY_OUTPUT_PATH}/RelWithDebInfo/libcrypto-1_1.dll>"
    DESTINATION bin
    COMPONENT Common)

endif(WIN32)
