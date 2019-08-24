set(BOOST_COMPONENTS thread log log_setup filesystem date_time regex program_options)
find_package(Boost REQUIRED COMPONENTS ${BOOST_COMPONENTS})

file(GLOB VC_REDIST_FILES "${VC_REDIST}/*")

if(NOT VC_REDIST_FILES)
  message(FATAL_ERROR "No VC++ redistributives were found in provided VC_REDIST: ${VC_REDIST}")
endif()

install(FILES ${VC_REDIST_FILES}
  DESTINATION bin
  COMPONENT Common)