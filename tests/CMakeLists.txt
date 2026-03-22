add_executable(coap_server_dispatch_tests
    coap/server/test_edhoc_dispatch_with_deps.c
    ${CMAKE_SOURCE_DIR}/src/coap/server/edhoc_dispatch.c
)

target_include_directories(coap_server_dispatch_tests PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src/coap/server
    ${CMAKE_SOURCE_DIR}/externals/libedhoc/helpers/include
    ${CMAKE_SOURCE_DIR}/externals/libedhoc/include
)

target_link_libraries(coap_server_dispatch_tests PRIVATE
    libedhoc
    libcoap::coap-3
    unity
)

add_custom_target(run_unity_tests
    COMMAND coap_server_dispatch_tests
    DEPENDS coap_server_dispatch_tests
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Running Unity tests"
)
