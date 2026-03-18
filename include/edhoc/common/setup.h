#ifndef EDHOC_COMMON_SETUP_H_
#define EDHOC_COMMON_SETUP_H_

#include <edhoc.h>

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

/**
 * @brief Initialize credential key information in EDHOC credentials structure
 * @param credentials Pointer to EDHOC credentials structure
 * @param public_key Public key bytes
 * @param public_key_length Length of public key
 * @param key_id_integer Key identifier as integer
 */
void initialize_credential_key(struct edhoc_auth_creds* credentials,
                               const uint8_t* public_key,
                               size_t public_key_length,
                               int32_t key_id_integer);

#endif  // EDHOC_COMMON_SETUP_H_
