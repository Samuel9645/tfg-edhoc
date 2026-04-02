# 1. Setup Unity Paths and Threads
set(UNITY_SCRIPT_DIR ${CMAKE_SOURCE_DIR}/externals/unity/auto)
find_package(Threads REQUIRED)

# 2. Define your source groups (keep these as handy variables)
set(CORE_SOURCES
    ${SERVER_SOURCES}
    ${CLIENT_SOURCES}
)

# Suppress warnings from helpers
set_source_files_properties(${CMAKE_SOURCE_DIR}/externals/libedhoc/helpers/src/edhoc_cipher_suite_2.c PROPERTIES COMPILE_FLAGS "-w")


# ==============================================================================
# 3. DEFINE THE TEST GENERATOR FUNCTION
# ==============================================================================
# This function takes the Target Name, the Test File, and then any number
# of extra source files or stubs (${ARGN}) to link into the executable.
function(add_unity_test TARGET_NAME TEST_FILE)
  set(TEST_SRC "${CMAKE_CURRENT_SOURCE_DIR}/${TEST_FILE}")
  set(TEST_RUNNER "${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}_Runner.c")

  # A. Generate the Unity Runner
  add_custom_command(
      OUTPUT ${TEST_RUNNER}
      COMMAND ruby ${UNITY_SCRIPT_DIR}/generate_test_runner.rb ${TEST_SRC} ${TEST_RUNNER}
      DEPENDS ${TEST_SRC}
      COMMENT "Generating Unity runner for ${TARGET_NAME}"
  )

  # B. Create the executable using the test, runner, and extra args (${ARGN})
  add_executable(${TARGET_NAME}
      ${TEST_SRC}
      ${TEST_RUNNER}
      ${ARGN}
  )

  # C. Standard Include Directories for all tests
  target_include_directories(${TARGET_NAME} PRIVATE
      ${CMAKE_SOURCE_DIR}/include                      # For "core/server.h"
      ${CMAKE_SOURCE_DIR}/src                          # For "coap/server/internal/..."
      ${CMAKE_CURRENT_SOURCE_DIR}/include              # For test headers / stubs
      ${CMAKE_SOURCE_DIR}/externals/libedhoc/include
      ${CMAKE_SOURCE_DIR}/externals/libedhoc/helpers/include
  )

  # D. Standard Link Libraries for all tests
  target_link_libraries(${TARGET_NAME} PRIVATE
      libedhoc
      libcoap::coap-3
      unity
      Threads::Threads
  )

  target_compile_options(${TARGET_NAME} PRIVATE ${CUSTOM_COMPILE_FLAGS})
  target_link_options(${TARGET_NAME} PRIVATE ${MEMORY_CHECK_FLAGS})

  # E. Register with CTest
  add_test(NAME ${TARGET_NAME} COMMAND ${TARGET_NAME})

  set_property(TEST ${TARGET_NAME} APPEND PROPERTY ENVIRONMENT "ASAN_OPTIONS=halt_on_error=1:abort_on_error=1")
  set_property(TEST ${TARGET_NAME} APPEND PROPERTY ENVIRONMENT "UBSAN_OPTIONS=halt_on_error=1:abort_on_error=1:print_stacktrace=1")
endfunction()


# ==============================================================================
# 4. REGISTER TESTS WITH GRANULAR CONTROL
# ==============================================================================

add_unity_test(test_dispatch
    /src/coap/server/dispatch/test_dispatch.c
    ${SERVER_SOURCES}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/coap/server/dispatch/stubs.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/coap/server/dispatch/create_base_dependencies.c
)

add_unity_test(test_message_1_parser
    /src/edhoc/server/handshake/message_1/parser/test_message_1_parser.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/edhoc/server/handshake/payload.c
    ${CMAKE_SOURCE_DIR}/src/edhoc/server/handshake/message_1/parser.c
)

add_unity_test(test_message_1_handler
    /src/edhoc/server/handshake/message_1/handler/test_message_1_handler.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/edhoc/server/handshake/message_1/handler/helpers.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/edhoc/server/handshake/message_1/handler/stubs.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/edhoc/server/handshake/payload.c
    ${CMAKE_SOURCE_DIR}/src/edhoc/server/handshake/message_1/handler.c
    ${CMAKE_SOURCE_DIR}/src/edhoc/server/handshake/message_1/result.c
    ${CMAKE_SOURCE_DIR}/src/edhoc/server/handshake/common/request_data.c
    ${CMAKE_SOURCE_DIR}/src/common/data_models.c
)

add_unity_test(test_message_3_handler
    /src/edhoc/server/handshake/message_3/handler/test_message_3_handler.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/edhoc/server/handshake/message_3/handler/helpers.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/edhoc/server/handshake/message_3/handler/stubs.c
    ${CMAKE_SOURCE_DIR}/src/edhoc/server/handshake/message_3/handler.c
    ${CMAKE_SOURCE_DIR}/src/edhoc/server/handshake/common/request_data.c
    ${CMAKE_SOURCE_DIR}/src/common/data_models.c
)

add_unity_test(test_message_3_parser
    /src/edhoc/server/handshake/message_3/parser/test_message_3_parser.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/edhoc/server/handshake/message_3/parser/stubs.c
    ${CMAKE_SOURCE_DIR}/src/edhoc/server/handshake/message_3/parser.c
)

add_unity_test(test_integration
    /src/core/test_integration.c
    ${CORE_SOURCES}
)

# Apply the specific compile definitions ONLY to test_integration
target_compile_definitions(test_integration PRIVATE
    SERVER_PATH="$<TARGET_FILE:server>"
    CLIENT_PATH="$<TARGET_FILE:client>"
)