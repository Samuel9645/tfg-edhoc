# Third-party dependency entrypoint.
# Keep add_subdirectory ordering stable: Unity first, then projects that may
# optionally create their own unity target.

if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/unity/CMakeLists.txt)
  add_subdirectory(unity)
else()
  message(FATAL_ERROR
      "Missing Unity submodule at ${CMAKE_CURRENT_LIST_DIR}/unity. "
      "Run: git submodule update --init --recursive")
endif()

if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/libedhoc/CMakeLists.txt)
  add_subdirectory(libedhoc)
else()
  message(FATAL_ERROR
      "Missing libedhoc submodule at ${CMAKE_CURRENT_LIST_DIR}/libedhoc. "
      "Run: git submodule update --init --recursive")
endif()

if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/libcoap/CMakeLists.txt)
  add_subdirectory(libcoap)
else()
  message(FATAL_ERROR
      "Missing libcoap submodule at ${CMAKE_CURRENT_LIST_DIR}/libcoap. "
      "Run: git submodule update --init --recursive")
endif()
