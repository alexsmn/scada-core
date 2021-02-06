file(GLOB VC_REDIST_FILES "${VC_REDIST}/*")

if(NOT VC_REDIST_FILES)
  message(FATAL_ERROR "No VC++ redistributives were found in provided VC_REDIST: ${VC_REDIST}")
endif()

install(FILES ${VC_REDIST_FILES}
  DESTINATION bin
  COMPONENT Common)
