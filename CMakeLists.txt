cmake_minimum_required(VERSION 3.13)
project(EDHOC_Demo C)

# --- FIX: Must be C11 to allow unnamed structs/unions! ---
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_EXTENSIONS ON)

include(cmake_configs/libedhoc_config.cmake)

# Third-party dependencies are managed in externals/.
add_subdirectory(externals)

set(LIBEDHOC_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/externals/libedhoc/helpers/src/edhoc_cipher_suite_2.c
    ${CMAKE_CURRENT_SOURCE_DIR}/externals/libedhoc/helpers/src/edhoc_helpers.c
)

set(ADD_EDHOC_ERROR_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/edhoc/common/add_error/com_edhoc_add_internal_error.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/edhoc/common/add_error/com_edhoc_add_protocol_error.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/edhoc/common/add_error/internal/com_edhoc_set_error_info.c
)

set(BASE_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common/com_data_models.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common/com_session_resources.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/edhoc/common/com_edhoc_setup.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/edhoc/edhoc_config.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/edhoc/credentials/cred_pub_data.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/edhoc/credentials/cred_auth.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/coap/common/com_coap_context.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/coap/common/com_coap_get_data.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/coap/common/com_coap_helpers.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/coap/common/com_coap_response.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/coap/common/com_coap_parse_edhoc_request.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/coap/common/internal/com_parse_edhoc_request_builders.c
    ${ADD_EDHOC_ERROR_SOURCES}
    ${LIBEDHOC_SOURCES}
)

set(MESSAGE_1_PROCESS_SOURCES
    ${CMAKE_SOURCE_DIR}/src/edhoc/server/handshake/message_1/internal/srv_m1_process_result_builders.c
    ${CMAKE_SOURCE_DIR}/src/edhoc/server/handshake/message_1/srv_m1_process.c
    ${CMAKE_SOURCE_DIR}/src/edhoc/server/handshake/message_1/srv_m1_errors.c
    ${CMAKE_SOURCE_DIR}/externals/libedhoc/helpers/src/edhoc_cipher_suite_2.c
    ${CMAKE_SOURCE_DIR}/src/edhoc/common/com_edhoc_setup.c
)

set(MESSAGE_2_COMPOSE_SOURCES
    ${CMAKE_SOURCE_DIR}/src/edhoc/server/handshake/message_2/srv_m2_compose.c
    ${CMAKE_SOURCE_DIR}/src/edhoc/server/handshake/message_2/internal/srv_m2_compose_result_builders.c
)

set(MESSAGE_1_RESPONDER_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/edhoc/server/handshake/message_1/srv_m1_responder.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/edhoc/server/handshake/message_1/srv_m1_parser.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/edhoc/server/handshake/message_1/internal/srv_m1_parser_result_builders.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/edhoc/server/handshake/message_1/internal/srv_m1_responder_result_builders.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/edhoc/server/handshake/message_1/srv_m1_errors.c
    ${MESSAGE_1_PROCESS_SOURCES}
    ${MESSAGE_2_COMPOSE_SOURCES}
)

set(MESSAGE_3_PROCESS_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/edhoc/server/handshake/message_3/srv_m3_process.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/edhoc/server/handshake/message_3/internal/srv_m3_process_result_builders.c
)

set(MESSAGE_4_COMPOSE_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/edhoc/server/handshake/message_4/srv_m4_compose.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/edhoc/server/handshake/message_4/internal/srv_m4_compose_result_builders.c
)

set(MESSAGE_3_RESPONDER_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/edhoc/server/handshake/message_3/srv_m3_responder.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/edhoc/server/handshake/message_3/internal/srv_m3_responder_result_builders.c
    ${MESSAGE_3_PROCESS_SOURCES}
    ${MESSAGE_4_COMPOSE_SOURCES}
)

set(SERVER_SPECIFIC_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/app_server.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/coap/server/srv_utils.c
    ${MESSAGE_1_RESPONDER_SOURCES}
    ${MESSAGE_3_RESPONDER_SOURCES}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/edhoc/server/handshake/message_3/srv_m3_parser.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/edhoc/server/handshake/message_3/internal/srv_m3_parser_result_builders.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/coap/server/edhoc_message_process/srv_coap_m1_process.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/coap/server/edhoc_message_process/srv_coap_m3_process.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/coap/server/srv_dispatch.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/coap/server/internal/srv_dispatch_engine.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/edhoc/credentials/cred_srv.c
)

set(SERVER_SOURCES
    ${BASE_SOURCES}
    ${SERVER_SPECIFIC_SOURCES}
)

set(CLIENT_SPECIFIC_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/app_client.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/coap/client/cli_resources.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/coap/client/cli_utils.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/coap/client/cli_log_error.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/coap/client/cli_exchange.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/edhoc/client/cli_handshake.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/edhoc/client/handshake/common/cli_state.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/edhoc/client/handshake/message_1/cli_m1_compose.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/edhoc/client/handshake/message_2/cli_m2_process.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/edhoc/client/handshake/message_3/cli_m3_compose.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/edhoc/client/handshake/message_4/cli_m4_process.c
)

set(CLIENT_SOURCES
    ${BASE_SOURCES}
    ${CLIENT_SPECIFIC_SOURCES}
)

# Define main executable
add_executable(server
    ${SERVER_SOURCES}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main_server.c
)
add_executable(client
    ${CLIENT_SOURCES}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main_client.c
)

# Link libraries
target_link_libraries(server PRIVATE libedhoc libcoap::coap-3)
target_link_libraries(client PRIVATE libedhoc libcoap::coap-3)

# Suppress warnings from helpers
set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/externals/libedhoc/helpers/src/edhoc_cipher_suite_2.c PROPERTIES COMPILE_FLAGS "-w")

# Add helper include directories. SYSTEM is set to suppress warnings from external
# code
target_include_directories(server PRIVATE include)
target_include_directories(server PRIVATE src)
target_include_directories(server SYSTEM PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/externals/libedhoc/helpers/include)
target_include_directories(client PRIVATE include)
target_include_directories(client PRIVATE src)
target_include_directories(client SYSTEM PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/externals/libedhoc/helpers/include)

# Set output directory for executables
set_target_properties(server PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
set_target_properties(client PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
set(MEMORY_CHECK_FLAGS "-fsanitize=address" "-fsanitize=undefined" "-fno-omit-frame-pointer" "-fno-sanitize-recover=all")
set(CUSTOM_COMPILE_FLAGS "-g" "-Wall" "-Wextra" "-Wpedantic" "-Werror" ${MEMORY_CHECK_FLAGS})

target_compile_options(server PRIVATE ${CUSTOM_COMPILE_FLAGS})
target_compile_options(client PRIVATE ${CUSTOM_COMPILE_FLAGS})
target_link_options(server PRIVATE ${MEMORY_CHECK_FLAGS})
target_link_options(client PRIVATE ${MEMORY_CHECK_FLAGS})

include(cmake_configs/testing_config.cmake)