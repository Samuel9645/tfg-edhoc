option(ENABLE_UNITY_TESTS "Build Unity test binaries" ON)

if (ENABLE_UNITY_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif()
