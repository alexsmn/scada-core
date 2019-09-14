set(BOOST_COMPONENTS system thread log log_setup filesystem date_time regex program_options)

find_package(Boost REQUIRED COMPONENTS ${BOOST_COMPONENTS})

if(WIN32)
  set(BOOST_SUFFIX "vc142-mt$<$<CONFIG:Debug>:-gd>-x32-${Boost_LIB_VERSION}")
  # TODO: Remove Vcpkg dependency.
  set(BOOST_DLL_DIR "${_VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}$<$<CONFIG:Debug>:/debug>/bin")

  macro(list_boost_dlls DLLS)
    foreach(BOOST_COMPONENT ${ARGN})
      list(APPEND ${DLLS} "${BOOST_DLL_DIR}/boost_${BOOST_COMPONENT}-${BOOST_SUFFIX}.dll")
    endforeach()
  endmacro()

  message("Install Boost components: ${BOOST_COMPONENTS}")
  list_boost_dlls(BOOST_DLLS ${BOOST_COMPONENTS})
  install(FILES ${BOOST_DLLS}
    DESTINATION bin
    COMPONENT Common)

endif(WIN32)
