cmake_minimum_required(VERSION 3.13)
project(EDHOC_Demo C)

# --- FIX: Must be C11 to allow unnamed structs/unions! ---
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_EXTENSIONS ON)

include(libedhoc_config.cmake)

# Add the libedhoc library
# (It will automatically handle its own fallback configs for Zephyr)
add_subdirectory(./libedhoc)

set(BASE_SOURCES
    src/edhoc_setup.c
    src/psa_setup.c
    src/public_data.c
    src/common_data.c
    src/shared_credentials.c
    src/shared_crypto.c
    libedhoc/helpers/src/edhoc_cipher_suite_2.c
)

# Define main executable
add_executable(server
    src/main_server.c
    src/server.c
    ${BASE_SOURCES}
)
add_executable(client
    src/main_client.c
    src/client.c
    ${BASE_SOURCES}
)
# Link libraries
target_link_libraries(server PRIVATE libedhoc)
target_link_libraries(client PRIVATE libedhoc)

# Add helper include directories
target_include_directories(server PRIVATE include)
target_include_directories(server PRIVATE libedhoc/helpers/include)
target_include_directories(client PRIVATE include)
target_include_directories(client PRIVATE libedhoc/helpers/include)

# Set output directory for executables
set_target_properties(server PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
set_target_properties(client PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
set(MEMORY_CHECK_FLAGS "-fsanitize=address" "-fsanitize=undefined")
set(CUSTOM_COMPILE_FLAGS "-g" "-Wall" "-Wextra" "-Wpedantic" ${MEMORY_CHECK_FLAGS})

target_compile_options(server PRIVATE ${CUSTOM_COMPILE_FLAGS}) 
target_compile_options(client PRIVATE ${CUSTOM_COMPILE_FLAGS})
target_link_options(server PRIVATE ${MEMORY_CHECK_FLAGS})
target_link_options(client PRIVATE ${MEMORY_CHECK_FLAGS})