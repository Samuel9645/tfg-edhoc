/**
 * @file test_edhoc_dispatch_deps_builder.h
 *
 * @brief Dependency-table builder for EDHOC dispatcher seam tests.
 *
 * Exposes a baseline dependency set that test cases can customize by replacing
 * selected function pointers.
 */
#ifndef TEST_EDHOC_DISPATCH_DEPS_BUILDER_H_
#define TEST_EDHOC_DISPATCH_DEPS_BUILDER_H_

#include "coap/server/internal/edhoc_dispatch_engine.h"
#include "coap/server/test_edhoc_dispatch_stubs.h"

/**
 * @brief Creates default dependency table backed by local test stubs.
 *
 * @return Initialized dependency table for
 * coap_server_dispatch_edhoc_post_with_deps().
 */
coap_server_edhoc_dispatch_deps_t make_base_deps(void);

#endif  // TEST_EDHOC_DISPATCH_DEPS_BUILDER_H_
