/**
 * @file /dispatch/create_base_dependencies.h
 *
 * @brief Dependency-table builder for EDHOC dispatcher seam tests.
 *
 * Exposes a baseline dependency set that test cases can customize by replacing
 * selected function pointers.
 */
#ifndef COAP_SERVER_DISPATCH_CREATE_BASE_DEPENDENCIES_H_
#define COAP_SERVER_DISPATCH_CREATE_BASE_DEPENDENCIES_H_

#include "coap/server/dispatch/stubs.h"
#include "coap/server/internal/edhoc_dispatch_engine.h"

/**
 * @brief Creates default dependency table backed by local test stubs.
 *
 * @return Initialized dependency table for
 * coap_server_dispatch_edhoc_post_with_dependencies().
 */
coap_server_edhoc_dispatch_deps_t create_base_dependencies(void);

#endif  // COAP_SERVER_DISPATCH_CREATE_BASE_DEPENDENCIES_H_
