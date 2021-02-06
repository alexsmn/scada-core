if(WIN32)
  set(BOOST_COMPONENTS thread log log_setup filesystem date_time regex program_options)
  find_package(Boost REQUIRED COMPONENTS ${BOOST_COMPONENTS})

  set(BOOST_DEBUG_SUFFIX "vc142-mt-gd-x32-${Boost_LIB_VERSION}")
  set(BOOST_RELEASE_SUFFIX "vc142-mt-x32-${Boost_LIB_VERSION}")

  foreach(BOOST_COMPONENT ${BOOST_COMPONENTS})
    if(NOT ${BOOST_COMPONENT} MATCHES "date_time")
      install(FILES
          "$<$<CONFIG:Debug>:${LIBRARY_OUTPUT_PATH}/Debug/boost_${BOOST_COMPONENT}-${BOOST_DEBUG_SUFFIX}.dll>"
          "$<$<CONFIG:RelWithDebInfo>:${LIBRARY_OUTPUT_PATH}/RelWithDebInfo/boost_${BOOST_COMPONENT}-${BOOST_RELEASE_SUFFIX}.dll>"
        DESTINATION bin
        COMPONENT Common)
    endif()
  endforeach()

endif(WIN32)