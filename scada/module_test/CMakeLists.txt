# Smoke tests for the scada.core module facade (SCADA_CXX_MODULES=ON only).
scada_add_module_smoke_test(scada_core SOURCES
  module_smoke_unittest.cpp   # import-only TU
  module_mixed_unittest.cpp   # import + #include of the same headers (ODR)
)
