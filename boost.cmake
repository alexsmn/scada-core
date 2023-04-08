set(BOOST_COMPONENTS atomic context coroutine thread log log_setup filesystem date_time program_options)

find_package(Boost REQUIRED COMPONENTS ${BOOST_COMPONENTS})

if(WIN32)
  if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(BOOST_ARCH "x64")
  else()
    set(BOOST_ARCH "x32")
  endif()
  

  set(BOOST_DEBUG_SUFFIX "vc143-mt-gd-${BOOST_ARCH}-${Boost_LIB_VERSION}")
  set(BOOST_RELEASE_SUFFIX "vc143-mt-${BOOST_ARCH}-${Boost_LIB_VERSION}")

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
