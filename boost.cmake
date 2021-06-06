set(BOOST_COMPONENTS thread log log_setup filesystem date_time regex program_options)

find_package(Boost REQUIRED COMPONENTS ${BOOST_COMPONENTS})

if(WIN32)
  set(BOOST_SUFFIX "vc142-mt$<$<CONFIG:Debug>:-gd>-x32-${Boost_LIB_VERSION}")

  set(BOOST_DLL_DIR "${LIBRARY_OUTPUT_PATH}/${CMAKE_BUILD_TYPE}")
  foreach(BOOST_COMPONENT ${BOOST_COMPONENTS})
    if(NOT ${BOOST_COMPONENT} MATCHES "date_time")
      list(APPEND BOOST_DLLS "${BOOST_DLL_DIR}/boost_${BOOST_COMPONENT}-${BOOST_SUFFIX}.dll")
    endif()
  endforeach()

  install(FILES ${BOOST_DLLS}
    DESTINATION bin
    COMPONENT Common)

endif(WIN32)
