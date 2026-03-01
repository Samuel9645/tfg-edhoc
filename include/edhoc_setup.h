#ifndef EDHOC_SETUP_H
#define EDHOC_SETUP_H

#include <netinet/in.h>
#include <psa/crypto.h>
#include <psa/crypto_types.h>
#include <psa/crypto_values.h>
#include <stdio.h>

#include "edhoc.h"
#include "edhoc_cipher_suite_2.h"
#include "edhoc_context.h"

/**
 * Initializes PSA crypto and configures EDHOC context with cipher suite,
 * methods, and credentials.
 * @param ctx Pointer to uninitialized edhoc_context
 * @param credentials Pointer to edhoc_credentials struct with fetch/verify
 * callbacks
 * @return EDHOC_SUCCESS on success, error code otherwise
 */
int edhoc_setup_context(struct edhoc_context* ctx,
                        const struct edhoc_credentials* credentials);

#endif
