add_executable(coap_server_dispatch_tests
    src/coap/server/test_edhoc_dispatch_with_deps.c
    src/coap/server/test_edhoc_dispatch_stubs.c
    src/coap/server/test_edhoc_dispatch_deps_builder.c
    ${CMAKE_SOURCE_DIR}/src/coap/server/internal/edhoc_dispatch_engine.c
)

target_include_directories(coap_server_dispatch_tests PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/tests/include
    ${CMAKE_SOURCE_DIR}/src/coap/server
    ${CMAKE_SOURCE_DIR}/externals/libedhoc/helpers/include
    ${CMAKE_SOURCE_DIR}/externals/libedhoc/include
)

target_link_libraries(coap_server_dispatch_tests PRIVATE
    libedhoc
    libcoap::coap-3
    unity
)

target_compile_options(coap_server_dispatch_tests PRIVATE -g -O0)

add_custom_target(run_unity_tests
    COMMAND coap_server_dispatch_tests
    DEPENDS coap_server_dispatch_tests
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Running Unity tests"
)
