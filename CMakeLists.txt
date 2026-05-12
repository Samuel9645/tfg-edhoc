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
    ${CMAKE_CURRENT_SOURCE_DIR}/externals/libedhoc/helpers/src/edhoc_cipher_suite_0.c
    ${CMAKE_CURRENT_SOURCE_DIR}/externals/libedhoc/helpers/src/edhoc_helpers.c
)
add_library(edhoc_external_helpers STATIC ${LIBEDHOC_SOURCES})
target_include_directories(edhoc_external_helpers SYSTEM PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/externals/libedhoc/helpers/include
    ${CMAKE_CURRENT_SOURCE_DIR}/externals/libedhoc/include
)
target_compile_options(edhoc_external_helpers PRIVATE -w)
target_link_libraries(edhoc_external_helpers PRIVATE libedhoc)

set(ADD_EDHOC_ERROR_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/edhoc/common/add_error/com_edhoc_add_internal_error.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/edhoc/common/add_error/com_edhoc_add_protocol_error.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/edhoc/common/add_error/internal/com_edhoc_set_error_info.c
)

set(EDHOC_CIPHER_SUITES_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/edhoc/common/com_edhoc_cipher_suites.c
)

set(EDHOC_CONTEXT_INITIALIZATION_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/edhoc/common/com_edhoc_setup_context.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/edhoc/common/com_edhoc_parameters.c
    ${EDHOC_CIPHER_SUITES_SOURCES}
)

set(COMMON_COAP_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/coap/common/com_coap_get_data.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/coap/common/com_coap_helpers.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/coap/common/com_coap_parse_edhoc_request.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/coap/common/internal/com_parse_edhoc_request_builders.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/coap/common/com_coap_response.c
)

set(BASE_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common/com_data_models.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common/com_session_resources.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/edhoc/credentials/cred_pub_data.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/edhoc/credentials/cred_auth.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/coap/common/com_coap_context.c
    ${COMMON_COAP_SOURCES}
    ${EDHOC_CONTEXT_INITIALIZATION_SOURCES}
    ${ADD_EDHOC_ERROR_SOURCES}
)

set(MESSAGE_1_PROCESS_SOURCES
    ${CMAKE_SOURCE_DIR}/src/edhoc/server/handshake/message_1/srv_m1_process.c
    ${EDHOC_CONTEXT_INITIALIZATION_SOURCES}
)

set(MESSAGE_2_COMPOSE_SOURCES
    ${CMAKE_SOURCE_DIR}/src/edhoc/server/handshake/message_2/srv_m2_compose.c
)

set(MESSAGE_1_RESPONDER_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/edhoc/server/handshake/message_1/srv_m1_responder.c
    ${MESSAGE_1_PROCESS_SOURCES}
    ${MESSAGE_2_COMPOSE_SOURCES}
)

set(MESSAGE_3_PROCESS_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/edhoc/server/handshake/message_3/srv_m3_process.c
)

set(MESSAGE_4_COMPOSE_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/edhoc/server/handshake/message_4/srv_m4_compose.c
)

set(MESSAGE_3_RESPONDER_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/edhoc/server/handshake/message_3/srv_m3_responder.c
    ${MESSAGE_3_PROCESS_SOURCES}
    ${MESSAGE_4_COMPOSE_SOURCES}
)

set(COAP_EXTRACT_MESSAGE_1_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/coap/server/extract_edhoc_message/internal/srv_coap_extract_m1_result_builders.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/coap/server/extract_edhoc_message/srv_coap_extract_m1.c
)
set(COAP_EXTRACT_MESSAGE_3_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/coap/server/extract_edhoc_message/internal/srv_coap_extract_m3_result_builders.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/coap/server/extract_edhoc_message/srv_coap_extract_m3.c
)

set(SERVER_SPECIFIC_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/app_server.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/coap/server/srv_utils.c
    ${MESSAGE_1_RESPONDER_SOURCES}
    ${MESSAGE_3_RESPONDER_SOURCES}
    ${COAP_EXTRACT_MESSAGE_1_SOURCES}
    ${COAP_EXTRACT_MESSAGE_3_SOURCES}
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

set(CLIENT_EXCHANGE_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/coap/client/cli_resources.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/coap/client/cli_utils.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/coap/client/cli_log_error.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/coap/client/cli_exchange.c
)

set(CLIENT_SPECIFIC_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/app_client.c
    ${CLIENT_EXCHANGE_SOURCES}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/edhoc/client/handshake/message_1/cli_m1_compose.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/edhoc/client/handshake/cli_negotiate_cipher_suites.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/edhoc/client/handshake/message_2/cli_m2_process.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/edhoc/client/handshake/message_2/cli_m2_initiator.c
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
target_link_libraries(server PRIVATE edhoc_external_helpers libedhoc libcoap::coap-3)
target_link_libraries(client PRIVATE edhoc_external_helpers libedhoc libcoap::coap-3)

# Suppress warnings from helpers
set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/externals/libedhoc/helpers/src/edhoc_cipher_suite_2.c PROPERTIES COMPILE_FLAGS "-w")
set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/externals/libedhoc/helpers/src/edhoc_cipher_suite_0.c PROPERTIES COMPILE_FLAGS "-Wno-error")

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