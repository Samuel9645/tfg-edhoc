add_executable(coap_server_dispatch_tests
    src/coap/server/dispatch/tests.c
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

target_link_libraries(coap_server_dispatch_tests PRIVATE
    libedhoc
    libcoap::coap-3
    unity
)

target_compile_options(coap_server_dispatch_tests PRIVATE -g -O0)
add_test(NAME DispatchEngineTests COMMAND coap_server_dispatch_tests)