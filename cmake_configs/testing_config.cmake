if (NOT CMAKE_BUILD_TYPE STREQUAL "Release")
  enable_testing()
  add_subdirectory(tests)
endif()
