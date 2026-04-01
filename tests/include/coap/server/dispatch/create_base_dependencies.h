/**
 * @file create_base_dependencies.h
 *
 * @brief Dependency-table builder for EDHOC dispatcher seam tests.
 *
 * Exposes a baseline dependency set that test cases can customize by replacing
 * selected function pointers.
 */
#ifndef COAP_SERVER_DISPATCH_CREATE_BASE_DEPENDENCIES_H_
#define COAP_SERVER_DISPATCH_CREATE_BASE_DEPENDENCIES_H_

#include "coap/server/internal/dispatch_engine.h"

/**
 * @brief Creates default dependency table backed by local test stubs.
 *
 * @return Initialized dependency table for
 * coap_server_dispatch_post_with_dependencies().
 */
cp_server_dispatch_deps_t test_cp_srv_dispatch_create_base_dependencies(void);

#endif  // COAP_SERVER_DISPATCH_CREATE_BASE_DEPENDENCIES_H_
