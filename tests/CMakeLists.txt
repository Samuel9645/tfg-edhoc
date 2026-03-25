# Define where the Unity Ruby script lives
set(UNITY_SCRIPT_DIR ${CMAKE_SOURCE_DIR}/externals/unity/auto)

set(DISPATCH_TEST_SOURCE src/coap/server/dispatch/test_dispatch.c)
set(DISPATCH_TEST_RUNNER ${CMAKE_CURRENT_BINARY_DIR}/test_dispatch_Runner.c)

# Auto-generate the runner for dispatch tests
add_custom_command(
    OUTPUT ${DISPATCH_TEST_RUNNER}
    COMMAND ruby ${UNITY_SCRIPT_DIR}/generate_test_runner.rb ${CMAKE_CURRENT_SOURCE_DIR}/${DISPATCH_TEST_SOURCE} ${DISPATCH_TEST_RUNNER}
    DEPENDS ${DISPATCH_TEST_SOURCE}
    COMMENT "Auto-generating Unity test runner for ${DISPATCH_TEST_SOURCE}"
)

add_executable(coap_server_dispatch_tests 
    ${DISPATCH_TEST_SOURCE}
    ${DISPATCH_TEST_RUNNER}  
    src/coap/server/dispatch/stubs.c
    src/coap/server/dispatch/create_base_dependencies.c
    ${CMAKE_SOURCE_DIR}/src/coap/server/internal/edhoc_dispatch_engine.c
)

target_include_directories(coap_server_dispatch_tests PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/tests/include
    ${CMAKE_SOURCE_DIR}/src/coap/server
    ${CMAKE_SOURCE_DIR}/externals/libedhoc/helpers/include
    ${CMAKE_SOURCE_DIR}/externals/libedhoc/include
)

target_link_libraries(coap_server_dispatch_tests PRIVATE libedhoc libcoap::coap-3 unity)
target_compile_options(coap_server_dispatch_tests PRIVATE -g -O0)
add_test(NAME DispatchEngineTests COMMAND coap_server_dispatch_tests)