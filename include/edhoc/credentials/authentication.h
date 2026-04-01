/**
 * @file authentication.h
 *
 * @note The reason why it appears like the initialization is made twice
 * in the fetching and verifying is that the edhoc library uses the same
 * structure for both the own credentials and the peer credentials.
 * So in the fetch we initialize the structure with our own public key, and in
 * verify we initialize it with the peer public key. This is a design choice of
 * the library to simplify the handling of credentials, but it may seem a bit
 * redundant at first glance.
 */

#ifndef EDHOC_CREDENTIALS_AUTHENTICATION_H_
#define EDHOC_CREDENTIALS_AUTHENTICATION_H_

#include <edhoc_context.h>
#include <stdint.h>

/**
 * @brief Fetch and setup authentication credentials
 * @param[in] user_context User context (typically NULL)
 * @param[in] credentials Pointer to credentials structure to populate
 * @param[in] own_public_key Own public key bytes
 * @param[in] own_public_key_length Length of own public key
 * @param[in] own_private_key Own private key bytes
 * @param[in] own_private_key_length Length of own private key
 * @param[in] own_key_id Own key identifier as integer
 * @return EDHOC_SUCCESS on success, error code otherwise
 */
int edhoc_credentials_fetch(void* user_context,
                            struct edhoc_auth_creds* credentials,
                            const uint8_t* own_public_key,
                            size_t own_public_key_length,
                            const uint8_t* own_private_key,
                            size_t own_private_key_length, int32_t own_key_id);

/**
 * @brief Verify peer's authentication credentials
 * @param[in] user_context User context (typically NULL)
 * @param[in] credentials Peer's credential structure to verify
 * @param[in] expected_key_id Expected key identifier of peer
 * @param[in] peer_public_key Peer's public key bytes
 * @param[in] peer_public_key_length Length of peer's public key
 * @param[out] public_key_reference Reference to peer's public key
 * @param[out] public_key_length Output length of peer's public key
 * @return EDHOC_SUCCESS on success, error code otherwise
 *
 */
int edhoc_credentials_verify(const void* user_context,
                             struct edhoc_auth_creds* credentials,
                             int32_t expected_key_id,
                             const uint8_t* peer_public_key,
                             size_t peer_public_key_length,
                             const uint8_t** public_key_reference,
                             size_t* public_key_length);

#endif  // EDHOC_CREDENTIALS_AUTHENTICATION_H_