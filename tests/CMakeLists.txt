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

  # 1. Separate sources from linker flags (ARGN)
  set(EXTRA_SOURCES "")
  set(LINKER_FLAGS "")
  foreach (arg ${ARGN})
    if (${arg} MATCHES "^-Wl,")
      list(APPEND LINKER_FLAGS ${arg})
    else ()
      list(APPEND EXTRA_SOURCES ${arg})
    endif ()
  endforeach ()

  # 2. Generate the Unity Runner
  add_custom_command(
      OUTPUT ${TEST_RUNNER}
      COMMAND ruby ${UNITY_SCRIPT_DIR}/generate_test_runner.rb ${TEST_SRC} ${TEST_RUNNER}
      DEPENDS ${TEST_SRC}
      COMMENT "Generating Unity runner for ${TARGET_NAME}"
  )

  # 3. Create the executable
  add_executable(${TARGET_NAME}
      ${TEST_SRC}
      ${TEST_RUNNER}
      ${EXTRA_SOURCES}
  )

  # 4. RESTORED INCLUDE DIRECTORIES
  target_include_directories(${TARGET_NAME} PRIVATE
      ${CMAKE_SOURCE_DIR}/include                      # For "app/server.h"
      ${CMAKE_SOURCE_DIR}/src                          # For "edhoc/common/..." and "common/..."
      ${CMAKE_CURRENT_SOURCE_DIR}/include              # For test headers / stubs
      ${CMAKE_SOURCE_DIR}/externals/libedhoc/include
      ${CMAKE_SOURCE_DIR}/externals/libedhoc/helpers/include
  )

  # 5. Link Libraries
  target_link_libraries(${TARGET_NAME} PRIVATE
      libedhoc
      libcoap::coap-3
      unity
      Threads::Threads
  )

  # 6. Apply Linker Options (The --wrap logic)
  if (LINKER_FLAGS)
    target_link_options(${TARGET_NAME} PRIVATE ${LINKER_FLAGS})
  endif ()

  # 7. Standard Options
  target_compile_options(${TARGET_NAME} PRIVATE ${CUSTOM_COMPILE_FLAGS})
  target_link_options(${TARGET_NAME} PRIVATE ${MEMORY_CHECK_FLAGS})
  add_test(NAME ${TARGET_NAME} COMMAND ${TARGET_NAME})

  set_property(TEST ${TARGET_NAME} APPEND PROPERTY ENVIRONMENT "ASAN_OPTIONS=halt_on_error=1:abort_on_error=1")
  set_property(TEST ${TARGET_NAME} APPEND PROPERTY ENVIRONMENT "UBSAN_OPTIONS=halt_on_error=1:abort_on_error=1:print_stacktrace=1")
endfunction()


# ==============================================================================
# 4. REGISTER TESTS WITH GRANULAR CONTROL
# ==============================================================================


set(COMMON_SOURCES
    ${CMAKE_SOURCE_DIR}/src/common/com_data_models.c
)

set(COMMON_SRV_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common/tst_report_mock_error.c
    ${COMMON_SOURCES}
)

set(ADD_EDHOC_ERROR_COMMON_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/edhoc/common/add_error/common/tst_edhoc_add_error_assertions.c
    ${COMMON_SOURCES}
)

set(ADD_EDHOC_ERROR_INTEGRATION_SOURCES
    ${ADD_EDHOC_ERROR_COMMON_SOURCES}
)

set(ADD_PROTOCOL_ERROR_SOURCES
    ${CMAKE_SOURCE_DIR}/src/edhoc/common/add_error/com_edhoc_add_protocol_error.c
    ${CMAKE_SOURCE_DIR}/src/edhoc/common/add_error/internal/com_edhoc_set_error_info.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/edhoc/common/add_error/environments/tst_edhoc_add_protocol_error_env.c
)

add_unity_test(test_edhoc_integration_add_protocol_error
    /src/edhoc/common/add_error/integration/test_edhoc_integration_add_protocol_error.c
    ${ADD_PROTOCOL_ERROR_SOURCES}
    ${ADD_EDHOC_ERROR_INTEGRATION_SOURCES}
)

set(MOCK_EDHOC_ERROR_GET_CODE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/edhoc/common/add_error/mocks/tst_mock_edhoc_error_get_code.c
    "-Wl,--wrap=edhoc_error_get_code"
)

set(MOCK_EDHOC_ERROR_COMPOSE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/edhoc/common/add_error/mocks/tst_mock_edhoc_error_compose.c
    "-Wl,--wrap=edhoc_message_error_compose"
)

set(ADD_EDHOC_ERROR_UNIT_SOURCES
    ${MOCK_EDHOC_ERROR_COMPOSE}
    ${ADD_EDHOC_ERROR_COMMON_SOURCES}
)

add_unity_test(test_edhoc_unit_add_protocol_error
    /src/edhoc/common/add_error/unit/test_edhoc_unit_add_protocol_error.c
    ${ADD_PROTOCOL_ERROR_SOURCES}
    ${ADD_EDHOC_ERROR_UNIT_SOURCES}
    ${MOCK_EDHOC_ERROR_GET_CODE}
)

set(ADD_INTERNAL_ERROR_SOURCES
    ${CMAKE_SOURCE_DIR}/src/edhoc/common/add_error/com_edhoc_add_internal_error.c
    ${CMAKE_SOURCE_DIR}/src/edhoc/common/add_error/internal/com_edhoc_set_error_info.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/edhoc/common/add_error/environments/tst_edhoc_add_internal_error_env.c
)

add_unity_test(test_edhoc_integration_add_internal_error
    /src/edhoc/common/add_error/integration/test_edhoc_integration_add_internal_error.c
    ${ADD_EDHOC_ERROR_INTEGRATION_SOURCES}
    ${ADD_INTERNAL_ERROR_SOURCES}
)

add_unity_test(test_edhoc_unit_add_internal_error
    /src/edhoc/common/add_error/unit/test_edhoc_unit_add_internal_error.c
    ${ADD_EDHOC_ERROR_UNIT_SOURCES}
    ${ADD_INTERNAL_ERROR_SOURCES}
)

set(MESSAGE_1_PARSER_MOCK
    ${CMAKE_CURRENT_SOURCE_DIR}/src/edhoc/server/handshake/mocks/message_1/tst_srv_mock_edhoc_extract_flow_info.c
)

add_unity_test(test_edhoc_message_1_parser
    /src/edhoc/server/handshake/message_1/parser/test_edhoc_message_1_parser.c
    ${CMAKE_SOURCE_DIR}/src/edhoc/server/handshake/message_1/srv_m1_parser.c
    ${MESSAGE_1_PARSER_MOCK}
    ${ADD_EDHOC_ERROR_COMMON_SOURCES}
    ${CMAKE_SOURCE_DIR}/src/edhoc/common/add_error/com_edhoc_add_internal_error.c
    ${CMAKE_SOURCE_DIR}/src/edhoc/common/add_error/internal/com_edhoc_set_error_info.c
    ${COMMON_SRV_SOURCES}
)

set(COMMON_SOURCES
    ${COMMON_SRV_SOURCES}
    ${MOCK_EDHOC_ERROR_GET_CODE}
    ${CMAKE_SOURCE_DIR}/src/edhoc/common/add_error/com_edhoc_add_protocol_error.c
    ${CMAKE_SOURCE_DIR}/src/edhoc/common/add_error/internal/com_edhoc_set_error_info.c
    ${CMAKE_SOURCE_DIR}/src/edhoc/common/add_error/com_edhoc_add_internal_error.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/edhoc/common/add_error/common/tst_edhoc_add_error_assertions.c
)

set(HANDSHAKE_ERROR_SUPPORT_SOURCES
    ${ADD_EDHOC_ERROR_COMMON_SOURCES}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common/tst_report_mock_error.c
    ${CMAKE_SOURCE_DIR}/src/edhoc/common/add_error/com_edhoc_add_protocol_error.c
    ${CMAKE_SOURCE_DIR}/src/edhoc/common/add_error/internal/com_edhoc_set_error_info.c
    ${CMAKE_SOURCE_DIR}/src/edhoc/common/add_error/com_edhoc_add_internal_error.c
    ${ADD_EDHOC_ERROR_UNIT_SOURCES}
    ${MOCK_EDHOC_ERROR_GET_CODE}
)

set(EDHOC_CONTENT_INIT_MOCK
    src/edhoc/server/handshake/mocks/message_1/tst_srv_mock_edhoc_context_init.c
    "-Wl,--wrap=edhoc_context_init"
)

set(EDHOC_MESSAGE_1_PROCESS_MOCK
    src/edhoc/server/handshake/mocks/message_1/tst_srv_mock_edhoc_message_1_process.c
    "-Wl,--wrap=edhoc_message_1_process"
)

set(MESSAGE_1_PROCESS_MOCKS
    ${EDHOC_CONTENT_INIT_MOCK}
    ${EDHOC_MESSAGE_1_PROCESS_MOCK}
)

add_unity_test(test_edhoc_message_1_process
    /src/edhoc/server/handshake/message_1/process/test_edhoc_message_1_process.c
    ${MESSAGE_1_PROCESS_SOURCES}
    ${MESSAGE_1_PROCESS_MOCKS}
    ${HANDSHAKE_ERROR_SUPPORT_SOURCES}
)

set(MESSAGE_2_COMPOSE_MOCKS
    src/edhoc/server/handshake/mocks/message_2/tst_srv_mock_edhoc_message_2_compose.c
    "-Wl,--wrap=edhoc_message_2_compose"
)

add_unity_test(test_edhoc_message_2_compose
    /src/edhoc/server/handshake/message_2/test_edhoc_message_2_compose.c
    ${MESSAGE_2_COMPOSE_SOURCES}
    ${MESSAGE_2_COMPOSE_MOCKS}
    ${HANDSHAKE_ERROR_SUPPORT_SOURCES}
)

add_unity_test(test_edhoc_message_1_responder
    /src/edhoc/server/handshake/message_1/responder/test_edhoc_message_1_responder.c
    ${CMAKE_SOURCE_DIR}/src/edhoc/server/handshake/message_1/srv_m1_responder.c
    ${CMAKE_SOURCE_DIR}/src/edhoc/server/handshake/message_1/srv_m1_parser.c
    ${MESSAGE_1_PARSER_MOCK}
    ${MESSAGE_1_PROCESS_SOURCES}
    ${MESSAGE_1_PROCESS_MOCKS}
    ${MESSAGE_2_COMPOSE_SOURCES}
    ${MESSAGE_2_COMPOSE_MOCKS}
    ${HANDSHAKE_ERROR_SUPPORT_SOURCES}
)

#add_unity_test(test_edhoc_message_1_errors
#    /src/edhoc/server/handshake/message_1/errors/test_edhoc_edhoc_message_1_errors.c
#    ${CMAKE_CURRENT_SOURCE_DIR}/src/edhoc/server/handshake/message_1/errors/tst_srv_m1_errors_stubs.c
#    ${CMAKE_SOURCE_DIR}/src/edhoc/server/handshake/message_1/srv_m1_errors.c
#    ${CMAKE_SOURCE_DIR}/src/edhoc/common/add_edhoc_error_info.c
#    ${COMMON_SRV_SOURCES}
#)

set(MESSAGE_3_PARSER_MOCK
    ${CMAKE_CURRENT_SOURCE_DIR}/src/edhoc/server/handshake/mocks/message_3/tst_srv_mock_m3_parser_deps.c

)

add_unity_test(test_edhoc_message_3_parser
    /src/edhoc/server/handshake/message_3/parser/test_edhoc_message_3_parser.c
    ${CMAKE_SOURCE_DIR}/src/edhoc/server/handshake/message_3/srv_m3_parser.c
    ${MESSAGE_3_PARSER_MOCK}
    ${ADD_EDHOC_ERROR_COMMON_SOURCES}
    ${CMAKE_SOURCE_DIR}/src/edhoc/common/add_error/com_edhoc_add_internal_error.c
    ${CMAKE_SOURCE_DIR}/src/edhoc/common/add_error/internal/com_edhoc_set_error_info.c
    ${COMMON_SRV_SOURCES}
)

set(EDHOC_MESSAGE_3_PROCESS_MOCK
    src/edhoc/server/handshake/mocks/message_3/tst_srv_mock_edhoc_message_3_process.c
    "-Wl,--wrap=edhoc_message_3_process"
)

set(EDHOC_MESSAGE_4_COMPOSE_MOCK
    src/edhoc/server/handshake/mocks/message_4/tst_srv_mock_edhoc_message_4_compose.c
    "-Wl,--wrap=edhoc_message_4_compose"
)

set(MESSAGE_3_PROCESS_MOCKS
    ${EDHOC_MESSAGE_3_PROCESS_MOCK}
)

set(MESSAGE_4_COMPOSE_MOCKS
    ${EDHOC_MESSAGE_4_COMPOSE_MOCK}
)

add_unity_test(test_edhoc_message_3_process
    /src/edhoc/server/handshake/message_3/process/test_edhoc_message_3_process.c
    ${MESSAGE_3_PROCESS_SOURCES}
    ${MESSAGE_3_PROCESS_MOCKS}
    ${HANDSHAKE_ERROR_SUPPORT_SOURCES}
)

add_unity_test(test_edhoc_message_4_compose
    /src/edhoc/server/handshake/message_4/test_edhoc_message_4_compose.c
    ${MESSAGE_4_COMPOSE_SOURCES}
    ${MESSAGE_4_COMPOSE_MOCKS}
    ${HANDSHAKE_ERROR_SUPPORT_SOURCES}
)

add_unity_test(test_edhoc_message_3_responder
    /src/edhoc/server/handshake/message_3/responder/test_edhoc_message_3_responder.c
    ${CMAKE_SOURCE_DIR}/src/edhoc/server/handshake/message_3/srv_m3_responder.c
    ${CMAKE_SOURCE_DIR}/src/edhoc/server/handshake/message_3/srv_m3_parser.c
    ${MESSAGE_3_PARSER_MOCK}
    ${MESSAGE_3_RESPONDER_SOURCES}
    ${MESSAGE_3_PROCESS_MOCKS}
    ${MESSAGE_4_COMPOSE_MOCKS}
    ${HANDSHAKE_ERROR_SUPPORT_SOURCES}
)

set(CLIENT_MESSAGE_1_COMPOSE_MOCKS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/edhoc/client/handshake/mocks/message_1/tst_cli_mock_edhoc_message_1_compose.c
    "-Wl,--wrap=edhoc_prepend_flow"
    "-Wl,--wrap=edhoc_message_1_compose"
    "-Wl,--wrap=edhoc_prepend_recalculate_size"
)

add_unity_test(test_edhoc_message_1_compose
    /src/edhoc/client/handshake/message_1/test_edhoc_message_1_compose.c
    ${CMAKE_SOURCE_DIR}/src/edhoc/client/handshake/message_1/cli_m1_compose.c
    ${CLIENT_MESSAGE_1_COMPOSE_MOCKS}
    ${HANDSHAKE_ERROR_SUPPORT_SOURCES}
)

set(CLIENT_MESSAGE_2_PROCESS_MOCKS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/edhoc/client/handshake/mocks/message_2/tst_cli_mock_edhoc_message_2_process.c
    "-Wl,--wrap=edhoc_message_2_process"
)

set(CLIENT_MESSAGE_2_PROCESS_SOURCES
    ${CMAKE_SOURCE_DIR}/src/edhoc/client/handshake/message_2/cli_m2_process.c
)

add_unity_test(test_edhoc_message_2_process
    /src/edhoc/client/handshake/message_2/process/test_edhoc_message_2_process.c
    ${CLIENT_MESSAGE_2_PROCESS_SOURCES}
    ${CLIENT_MESSAGE_2_PROCESS_MOCKS}
    ${HANDSHAKE_ERROR_SUPPORT_SOURCES}
)

set(CLIENT_MESSAGE_3_COMPOSE_MOCKS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/edhoc/client/handshake/mocks/message_3/tst_cli_mock_edhoc_message_3_compose.c
    "-Wl,--wrap=edhoc_prepend_connection_id"
    "-Wl,--wrap=edhoc_message_3_compose"
    "-Wl,--wrap=edhoc_prepend_recalculate_size"
)

set(CLIENT_MESSAGE_3_COMPOSE_SOURCES
    ${CMAKE_SOURCE_DIR}/src/edhoc/client/handshake/message_3/cli_m3_compose.c
)

add_unity_test(test_edhoc_message_3_compose
    /src/edhoc/client/handshake/message_3/test_edhoc_message_3_compose.c
    ${CLIENT_MESSAGE_3_COMPOSE_SOURCES}
    ${CLIENT_MESSAGE_3_COMPOSE_MOCKS}
    ${HANDSHAKE_ERROR_SUPPORT_SOURCES}
)

set(CLIENT_MESSAGE_4_PROCESS_MOCKS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/edhoc/client/handshake/mocks/message_4/tst_cli_mock_edhoc_message_4_process.c
    "-Wl,--wrap=edhoc_message_4_process"
)

set(CLIENT_MESSAGE_4_PROCESS_SOURCES
    ${CMAKE_SOURCE_DIR}/src/edhoc/client/handshake/message_4/cli_m4_process.c
)

add_unity_test(test_edhoc_message_4_process
    /src/edhoc/client/handshake/message_4/process/test_edhoc_message_4_process.c
    ${CLIENT_MESSAGE_4_PROCESS_SOURCES}
    ${CLIENT_MESSAGE_4_PROCESS_MOCKS}
    ${HANDSHAKE_ERROR_SUPPORT_SOURCES}
)

add_unity_test(test_edhoc_message_2_initiator
    /src/edhoc/client/handshake/message_2/initiator/test_edhoc_message_2_initiator.c
    ${CMAKE_SOURCE_DIR}/src/edhoc/client/handshake/message_2/cli_m2_initiator.c
    ${CLIENT_MESSAGE_2_PROCESS_SOURCES}
    ${CLIENT_MESSAGE_2_PROCESS_MOCKS}
    ${CLIENT_MESSAGE_3_COMPOSE_SOURCES}
    ${CLIENT_MESSAGE_3_COMPOSE_MOCKS}
    ${HANDSHAKE_ERROR_SUPPORT_SOURCES}
)


add_unity_test(test_coap_server_dispatch
    /src/coap/server/dispatch/test_coap_server_dispatch.c
    ${SERVER_SOURCES}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/coap/server/dispatch/tst_srv_dispatch_stubs.c
    ${CMAKE_SOURCE_DIR}/src/edhoc/server/handshake/message_1/internal/srv_m1_responder_result_builders.c
    ${CMAKE_SOURCE_DIR}/src/edhoc/server/handshake/message_3/internal/srv_m3_responder_result_builders.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/coap/server/dispatch/tst_create_base_dependencies.c
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
