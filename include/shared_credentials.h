#ifndef SHARED_CREDENTIALS_H
#define SHARED_CREDENTIALS_H

#include <stddef.h>
#include <stdint.h>

#include "edhoc_context.h"

/**
 * @brief Fetch and setup authentication credentials
 * @param user_context User context (typically NULL)
 * @param credentials Pointer to credentials structure to populate
 * @param public_key Public key bytes for this entity
 * @param public_key_length Length of public key
 * @param private_key Private key bytes for this entity
 * @param private_key_length Length of private key
 * @param key_id Key identifier for this entity
 * @return EDHOC_SUCCESS on success, error code otherwise
 */
int shared_credential_fetch(void* user_context,
                            struct edhoc_auth_creds* credentials,
                            const uint8_t* public_key, size_t public_key_length,
                            const uint8_t* private_key,
                            size_t private_key_length, int32_t key_id);

/**
 * @brief Verify peer's authentication credentials
 * @param user_context User context (typically NULL)
 * @param credentials Peer's credential structure to verify
 * @param expected_key_id Expected key identifier of peer
 * @param peer_public_key Peer's public key bytes
 * @param peer_public_key_length Length of peer's public key
 * @param[out] public_key_reference Reference to peer's public key
 * @param[out] public_key_length Output length of peer's public key
 * @return EDHOC_SUCCESS on success, error code otherwise
 */
int shared_credential_verify(void* user_context,
                             struct edhoc_auth_creds* credentials,
                             int32_t expected_key_id,
                             const uint8_t* peer_public_key,
                             size_t peer_public_key_length,
                             const uint8_t** public_key_reference,
                             size_t* public_key_length);

#endif
