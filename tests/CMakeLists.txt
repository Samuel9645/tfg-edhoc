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
    ${CMAKE_CURRENT_SOURCE_DIR}/src/coap/server/dispatch/tst_srv_dispatch_stubs.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/coap/server/dispatch/tst_create_base_dependencies.c
)

set(COMMON_SRV_SOURCES
    ${CMAKE_SOURCE_DIR}/src/common/data_models.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/edhoc/common/tst_response_buffer_helpers.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common/tst_report_mock_error.c
)

add_unity_test(test_message_1_parser
    /src/edhoc/server/handshake/message_1/parser/test_message_1_parser.c
    ${CMAKE_SOURCE_DIR}/src/edhoc/server/handshake/message_1/srv_m1_parser.c
    ${CMAKE_SOURCE_DIR}/src/edhoc/server/handshake/message_1/internal/srv_m1_parser_result_builders.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/edhoc/server/handshake/message_1/parser/tst_srv_mock_m1_parser_deps.c
    ${COMMON_SRV_SOURCES}
)

set(COMMON_HANDLER_SOURCES
    ${COMMON_SRV_SOURCES}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/edhoc/common/tst_mock_add_edhoc_error_info.c
)

add_unity_test(test_message_1_handler
    /src/edhoc/server/handshake/message_1/handler/test_message_1_handler.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/edhoc/server/handshake/message_1/handler/tst_srv_m1_hdl_env.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/edhoc/server/handshake/message_1/handler/tst_srv_mock_m1_hdl_deps.c
    ${CMAKE_SOURCE_DIR}/src/edhoc/server/handshake/message_1/srv_m1_handler.c
    ${CMAKE_SOURCE_DIR}/src/edhoc/server/handshake/message_1/internal/srv_m1_handler_result_builders.c
    ${COMMON_HANDLER_SOURCES}
)

#add_unity_test(test_message_1_errors
#    /src/edhoc/server/handshake/message_1/errors/test_srv_message_1_errors.c
#    ${CMAKE_CURRENT_SOURCE_DIR}/src/edhoc/server/handshake/message_1/errors/tst_srv_m1_errors_stubs.c
#    ${CMAKE_SOURCE_DIR}/src/edhoc/server/handshake/message_1/srv_m1_errors.c
#    ${CMAKE_SOURCE_DIR}/src/edhoc/common/add_edhoc_error_info.c
#    ${COMMON_SRV_SOURCES}
#)

add_unity_test(test_message_3_handler
    /src/edhoc/server/handshake/message_3/handler/test_message_3_handler.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/edhoc/server/handshake/message_3/handler/tst_srv_m3_hdl_env.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/edhoc/server/handshake/message_3/handler/tst_srv_mock_m3_hdl_deps.c
    ${CMAKE_SOURCE_DIR}/src/edhoc/server/handshake/message_3/srv_m3_handler.c
    ${COMMON_HANDLER_SOURCES}
)

add_unity_test(test_message_3_parser
    /src/edhoc/server/handshake/message_3/parser/test_message_3_parser.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/edhoc/server/handshake/message_3/parser/tst_srv_m3_parser_stubs.c
    ${CMAKE_SOURCE_DIR}/src/edhoc/server/handshake/message_3/srv_m3_parser.c
    ${CMAKE_SOURCE_DIR}/src/edhoc/server/handshake/message_3/internal/srv_m3_parser_result_builders.c
    ${COMMON_SRV_SOURCES}
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