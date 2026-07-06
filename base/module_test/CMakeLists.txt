# Smoke tests for the scada.base module facade (SCADA_CXX_MODULES=ON only).
# Lives outside scada_module()'s source GLOB so these *_unittest.cpp files do
# not leak into scada_base_unittests.
scada_add_module_smoke_test(scada_base SOURCES
  module_smoke_unittest.cpp   # import-only TU
  module_mixed_unittest.cpp   # import + #include of the same headers (ODR)
)
