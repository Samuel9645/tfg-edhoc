cmake_minimum_required(VERSION 3.13)
project(EDHOC_Demo C)

# --- FIX: Must be C11 to allow unnamed structs/unions! ---
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_EXTENSIONS ON)

include(cmake_configs/libedhoc_config.cmake)

# Third-party dependencies are managed in externals/.
add_subdirectory(externals)

set(BASE_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/edhoc/common/setup.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common/data_models.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/edhoc/edhoc_config.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/edhoc/credentials/public_data.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/edhoc/credentials/authentication.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common/cleanup.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/coap/common/helpers.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/coap/common/response.c
    ${CMAKE_CURRENT_SOURCE_DIR}/externals/libedhoc/helpers/src/edhoc_cipher_suite_2.c
    ${CMAKE_CURRENT_SOURCE_DIR}/externals/libedhoc/helpers/src/edhoc_helpers.c
)

set(MESSAGE_1_HANDLER_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/edhoc/server/handshake/message_1/handler.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/edhoc/server/handshake/message_1/parser.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/edhoc/server/handshake/message_1/errors.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/edhoc/server/handshake/message_1/result.c
)

set(MESSAGE_3_HANDLER_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/edhoc/server/handshake/message_3/handler.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/edhoc/server/handshake/message_3/parser.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/edhoc/server/handshake/message_3/errors.c
)

set(SERVER_SPECIFIC_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/server.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/coap/server/utils.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/coap/server/request.c
    ${MESSAGE_1_HANDLER_SOURCES}
    ${MESSAGE_3_HANDLER_SOURCES}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/coap/server/edhoc_mapper/message_1_mapper.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/coap/server/edhoc_mapper/message_3_mapper.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/coap/server/dispatch.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/coap/server/internal/dispatch_engine.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/edhoc/credentials/server_credentials.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/edhoc/server/handshake/common/handle_libedhoc_errors.c
)

set(SERVER_SOURCES
    ${BASE_SOURCES}
    ${SERVER_SPECIFIC_SOURCES}
)

set(CLIENT_SPECIFIC_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/client.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/coap/client/cleanup.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/coap/client/utils.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/coap/client/log_error_response.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/coap/client/exchange.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/edhoc/client/handshake.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common/data_models.c
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

# Add helper include directories SYSTEM is set to suppress warnings from external 
# code
target_include_directories(server PRIVATE include)
target_include_directories(server SYSTEM PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/externals/libedhoc/helpers/include)
target_include_directories(client PRIVATE include)
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