cmake_minimum_required(VERSION 3.13)
project(EDHOC_Demo C)

# --- FIX: Must be C11 to allow unnamed structs/unions! ---
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_EXTENSIONS ON)

# Disable mbedtls tests
set(ENABLE_TESTING OFF CACHE BOOL "Disable mbedtls tests" FORCE)

# Compiler options
option(LIBEDHOC_BUILD_COMPILER_GCC "Enable compiling library with gcc" ON)
option(LIBEDHOC_BUILD_COMPILER_CLANG "Enable compiling library with clang" OFF)

# Define libedhoc configuration defaults (from Kconfig)
if(NOT DEFINED CONFIG_LIBEDHOC_ENABLE)
    set(CONFIG_LIBEDHOC_ENABLE "")
endif()
if(NOT DEFINED CONFIG_LIBEDHOC_KEY_ID_LEN)
    set(CONFIG_LIBEDHOC_KEY_ID_LEN 4)
endif()
if(NOT DEFINED CONFIG_LIBEDHOC_MAX_NR_OF_CIPHER_SUITES)
    set(CONFIG_LIBEDHOC_MAX_NR_OF_CIPHER_SUITES 3)
endif()
if(NOT DEFINED CONFIG_LIBEDHOC_MAX_LEN_OF_CONN_ID)
    set(CONFIG_LIBEDHOC_MAX_LEN_OF_CONN_ID 1)
endif()
if(NOT DEFINED CONFIG_LIBEDHOC_MAX_LEN_OF_ECC_KEY)
    set(CONFIG_LIBEDHOC_MAX_LEN_OF_ECC_KEY 32)
endif()
if(NOT DEFINED CONFIG_LIBEDHOC_MAX_LEN_OF_MAC)
    set(CONFIG_LIBEDHOC_MAX_LEN_OF_MAC 32)
endif()
if(NOT DEFINED CONFIG_LIBEDHOC_MAX_NR_OF_EAD_TOKENS)
    set(CONFIG_LIBEDHOC_MAX_NR_OF_EAD_TOKENS 3)
endif()
if(NOT DEFINED CONFIG_LIBEDHOC_MAX_LEN_OF_CRED_KEY_ID)
    set(CONFIG_LIBEDHOC_MAX_LEN_OF_CRED_KEY_ID 1)
endif()
if(NOT DEFINED CONFIG_LIBEDHOC_MAX_LEN_OF_HASH_ALG)
    set(CONFIG_LIBEDHOC_MAX_LEN_OF_HASH_ALG 1)
endif()
if(NOT DEFINED CONFIG_LIBEDHOC_MAX_NR_OF_CERTS_IN_X509_CHAIN)
    set(CONFIG_LIBEDHOC_MAX_NR_OF_CERTS_IN_X509_CHAIN 2)
endif()

# Add the libedhoc library
# (It will automatically handle its own fallback configs for Zephyr)
add_subdirectory(./libedhoc)

# Define main executable
add_executable(server
    src/main_server.c
    src/server.c
    src/edhoc_setup.c
    src/psa_setup.c
    src/public_data.c
    src/common_data.c
    libedhoc/helpers/src/edhoc_cipher_suite_2.c
)
add_executable(client
    src/main_client.c
    src/client.c
    src/edhoc_setup.c
    src/psa_setup.c
    src/public_data.c
    src/common_data.c
    libedhoc/helpers/src/edhoc_cipher_suite_2.c
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

target_compile_options(server PRIVATE -g -Wall -Wextra -Wpedantic) 
target_compile_options(client PRIVATE -g -Wall -Wextra -Wpedantic)