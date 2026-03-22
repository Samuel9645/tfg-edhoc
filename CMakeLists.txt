cmake_minimum_required(VERSION 3.13)
project(EDHOC_Demo C)

# --- FIX: Must be C11 to allow unnamed structs/unions! ---
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_EXTENSIONS ON)

include(cmake_configs/libedhoc_config.cmake)

# Third-party dependencies are managed in externals/.
add_subdirectory(externals)

set(BASE_SOURCES
    src/edhoc/common/setup.c
    src/edhoc/common/config.c
    src/edhoc/credentials/public_data.c
    src/edhoc/credentials/authentication.c
    src/common/cleanup.c
    src/coap/common/helpers.c
    src/coap/shared/edhoc_request.c
    src/coap/shared/edhoc_response.c
    externals/libedhoc/helpers/src/edhoc_cipher_suite_2.c
    externals/libedhoc/helpers/src/edhoc_helpers.c
)

# Define main executable
add_executable(server
    src/main_server.c
    src/server.c
    src/coap/server/utils.c
    src/coap/server/map_edhoc_error_to_response.c
    src/coap/server/edhoc_dispatch.c
    src/edhoc/server/server_flow.c
    ${BASE_SOURCES}
)
add_executable(client
    src/main_client.c
    src/client.c
    src/coap/client/cleanup.c
    src/coap/client/utils.c
    src/coap/client/log_edhoc_error_response.c
    src/coap/client/edhoc_exchange.c
    src/edhoc/client/client_flow.c
    ${BASE_SOURCES}
)
# Link libraries
target_link_libraries(server PRIVATE libedhoc libcoap::coap-3)
target_link_libraries(client PRIVATE libedhoc libcoap::coap-3)

# Suppress warnings from helpers
set_source_files_properties(externals/libedhoc/helpers/src/edhoc_cipher_suite_2.c PROPERTIES COMPILE_FLAGS "-w")

# Add helper include directories SYSTEM is set to suppress warnings from external 
# code
target_include_directories(server PRIVATE include)
target_include_directories(server SYSTEM PRIVATE externals/libedhoc/helpers/include)
target_include_directories(client PRIVATE include)
target_include_directories(client SYSTEM PRIVATE externals/libedhoc/helpers/include)

# Set output directory for executables
set_target_properties(server PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
set_target_properties(client PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
# set(MEMORY_CHECK_FLAGS "-fsanitize=address" "-fsanitize=undefined")
set(CUSTOM_COMPILE_FLAGS "-g" "-Wall" "-Wextra" "-Wpedantic" ${MEMORY_CHECK_FLAGS})

target_compile_options(server PRIVATE ${CUSTOM_COMPILE_FLAGS}) 
target_compile_options(client PRIVATE ${CUSTOM_COMPILE_FLAGS})
target_link_options(server PRIVATE ${MEMORY_CHECK_FLAGS})
target_link_options(client PRIVATE ${MEMORY_CHECK_FLAGS})

include(cmake_configs/testing_config.cmake)