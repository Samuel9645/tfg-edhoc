/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 31/03/2026
 * @brief Declarations for the module to facilitate the creation of
 * edhoc_credentials members
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#ifndef EDHOC_CREDENTIALS_CRED_AUTH_H_
#define EDHOC_CREDENTIALS_CRED_AUTH_H_

#include <edhoc.h>

#include "edhoc/credentials/cred_pub_data.h"

/**
 * @brief Fetch and setup authentication credentials
 * @param[in] credentials Pointer to credentials structure to populate
 * @param own_key_bindings
 * @param[in] own_public_credentials Struct containing the public credentials
 * data (public key, key identifier...)
 * @param[in] own_private_key Own private key bytes
 * @param[in] own_private_key_length Length of own private key
 * @return EDHOC_SUCCESS on success, error code otherwise
 */
int cred_edhoc_auth_fetch(
    struct edhoc_auth_creds* credentials,
    const struct edhoc_keys* own_key_bindings,
    struct cred_public_credentials_data own_public_credentials,
    const uint8_t* own_private_key, size_t own_private_key_length);

/**
 * @brief Verify peer's authentication credentials
 * @param[in] credentials Peer's credential structure to verify
 * @param[in] expected_peer_credentials Struct containing peer's public
 * credentials data (public key, key identifier...)
 * @param[out] public_key_reference Reference to peer's public key
 * @param[out] public_key_length Output length of peer's public key
 * @return EDHOC_SUCCESS on success, error code otherwise
 *
 */
int cred_edhoc_auth_verify(
    struct edhoc_auth_creds* credentials,
    struct cred_public_credentials_data expected_peer_credentials,
    const uint8_t** public_key_reference, size_t* public_key_length);

#endif  // EDHOC_CREDENTIALS_CRED_AUTH_H_