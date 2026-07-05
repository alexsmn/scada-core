# Smoke tests for the scada.base module facade (SCADA_CXX_MODULES=ON only).
# Lives outside scada_module()'s source GLOB so these *_unittest.cpp files do
# not leak into scada_base_unittests.
add_executable(scada_base_module_unittests
  module_smoke_unittest.cpp   # import-only TU
  module_mixed_unittest.cpp   # import + #include of the same headers (ODR)
)
set_target_properties(scada_base_module_unittests PROPERTIES
  # The tree's policy version predates CMP0155, so ordinary sources are not
  # scanned for imports by default; importing TUs must opt in explicitly.
  CXX_SCAN_FOR_MODULES ON
  CXX_CPPCHECK ""  # cppcheck cannot parse importing TUs
  # ccache does not hash the imported BMI, so it can serve stale objects for
  # importing TUs (see the note on scada_base_module).
  CXX_COMPILER_LAUNCHER ""
  FOLDER ${scada_core_folder})
target_link_libraries(scada_base_module_unittests PRIVATE
  scada_base_module
  base_unittest)

include(GoogleTest)
# PRE_TEST for the same reason as scada_module_unittests: discovery works
# even when only specific targets were built.
gtest_discover_tests(scada_base_module_unittests DISCOVERY_MODE PRE_TEST
                     PROPERTIES TIMEOUT 60)
