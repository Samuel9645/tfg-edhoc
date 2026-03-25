# 1. Setup Unity Paths and Threads
set(UNITY_SCRIPT_DIR ${CMAKE_SOURCE_DIR}/externals/unity/auto)
find_package(Threads REQUIRED)

# 2. Define the code being tested (relative to project root)
# These are the actual implementation files from your main src folder
set(CORE_SOURCES
    ${SERVER_SOURCES}
    ${CLIENT_SOURCES}
)

set(COAP_SERVER_SOURCES
    ${CMAKE_SOURCE_DIR}/src/coap/server/utils.c
    ${CMAKE_SOURCE_DIR}/src/coap/server/dispatch.c
    ${CMAKE_SOURCE_DIR}/src/coap/server/internal/edhoc_dispatch_engine.c
    ${CMAKE_SOURCE_DIR}/src/coap/server/map_error_to_response.c
)

# Suppress warnings from helpers
set_source_files_properties(${CMAKE_SOURCE_DIR}/externals/libedhoc/helpers/src/edhoc_cipher_suite_2.c PROPERTIES COMPILE_FLAGS "-w")


# 3. List only the file paths for your tests (relative to tests/ folder)
set(MY_TEST_FILES
    "src/coap/server/dispatch/test_dispatch.c"
    "src/core/test_handshake.c"
)

# 4. The Loop to automate target creation
foreach(test_rel_path ${MY_TEST_FILES})
    # Extract the name (e.g., test_handshake) to use as the target name
    get_filename_component(test_target ${test_rel_path} NAME_WE)
    
    set(test_src "${CMAKE_CURRENT_SOURCE_DIR}/${test_rel_path}")
    set(test_runner "${CMAKE_CURRENT_BINARY_DIR}/${test_target}_Runner.c")

    # A. Generate the Unity Runner using the Ruby script
    add_custom_command(
        OUTPUT ${test_runner}
        COMMAND ruby ${UNITY_SCRIPT_DIR}/generate_test_runner.rb ${test_src} ${test_runner}
        DEPENDS ${test_src}
        COMMENT "Generating Unity runner for ${test_target}"
    )

    # B. Create the executable
    # We combine the test file, the runner, and all necessary source files
    add_executable(${test_target} 
        ${test_src} 
        ${test_runner}
        ${CORE_SOURCES}
        ${COAP_SERVER_SOURCES}
        # Include your test-specific helper implementations
        ${CMAKE_CURRENT_SOURCE_DIR}/src/coap/server/dispatch/stubs.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/coap/server/dispatch/create_base_dependencies.c
    )

    # C. Include Directories
    # This is where we fix the "No such file or directory" errors
    target_include_directories(${test_target} PRIVATE
        ${CMAKE_SOURCE_DIR}/include                      # For "core/server.h"
        ${CMAKE_SOURCE_DIR}/src                          # For "coap/server/internal/..."
        ${CMAKE_CURRENT_SOURCE_DIR}/include              # For "coap/server/dispatch/stubs.h"
        ${CMAKE_SOURCE_DIR}/externals/libedhoc/include
        ${CMAKE_SOURCE_DIR}/externals/libedhoc/helpers/include
    )

    # D. Link Libraries
    target_link_libraries(${test_target} PRIVATE 
        libedhoc 
        libcoap::coap-3 
        unity 
        Threads::Threads                                # Required for pthreads
    )

    target_compile_options(${test_target} PRIVATE ${CUSTOM_COMPILE_FLAGS}) 
    target_link_options(${test_target} PRIVATE ${MEMORY_CHECK_FLAGS})

    # E. Register with CTest
    add_test(NAME ${test_target} COMMAND ${test_target})
endforeach()


target_compile_definitions(test_handshake PRIVATE 
    SERVER_PATH="$<TARGET_FILE:server>"
    CLIENT_PATH="$<TARGET_FILE:client>"
)