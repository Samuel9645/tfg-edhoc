#ifndef EDHOC_SETUP_H
#define EDHOC_SETUP_H

#include "edhoc.h"

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
