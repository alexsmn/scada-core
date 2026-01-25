if(NOT DEFINED VC_REDIST OR VC_REDIST STREQUAL "")
  return()
endif()

file(GLOB VC_REDIST_FILES "${VC_REDIST}/*")

if(NOT VC_REDIST_FILES)
  message(WARNING "No VC++ redistributables found in VC_REDIST: ${VC_REDIST}")
  return()
endif()

install(FILES ${VC_REDIST_FILES}
  DESTINATION bin
  COMPONENT Common)
