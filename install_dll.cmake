macro(install_dll)
  set(OPTIONS "")
  set(ONE_VALUE_ARGS "COMPONENT")
  set(MULTI_VALUE_ARGS "FILES")
  cmake_parse_arguments(INSTALL_DLL "${OPTIONS}" "${ONE_VALUE_ARGS}" "${MULTI_VALUE_ARGS}" ${ARGN})

  foreach(NAME ${INSTALL_DLL_FILES})
    list(APPEND PATHS
        "$<$<CONFIG:Debug>:${LIBRARY_OUTPUT_PATH}/Debug/${NAME}>"
        "$<$<CONFIG:RelWithDebInfo>:${LIBRARY_OUTPUT_PATH}/RelWithDebInfo/${NAME}>"
    )
  endforeach()

  message("## PATHS=${PATHS}")

  install(FILES ${PATHS}
    DESTINATION bin
    COMPONENT ${INSTALL_DLL_COMPONENT})
endmacro()
