#ifndef EDHOC_COMMON_EDHOC_SETUP_H_
#define EDHOC_COMMON_EDHOC_SETUP_H_

#include <edhoc.h>

enum com_edhoc_setup_context_status {
  COM_EDHOC_SETUP_CTX_OK = 0,
  COM_EDHOC_SETUP_CTX_ERR_PSA_INIT,
  COM_EDHOC_SETUP_CTX_ERR_CONTEXT_INIT,
  COM_EDHOC_SETUP_CTX_ERR_SET_METHODS,
  COM_EDHOC_SETUP_CTX_ERR_SET_CIPHER_SUITES,
  COM_EDHOC_SETUP_CTX_ERR_SET_CONNECTION_ID,
  COM_EDHOC_SETUP_CTX_ERR_BIND_KEYS,
  COM_EDHOC_SETUP_CTX_ERR_BIND_CRYPTO,
  COM_EDHOC_SETUP_CTX_ERR_BIND_CREDENTIALS,
};

/**
 * Initializes PSA crypto and configures EDHOC context with cipher suite,
 * methods, and credentials.
 * @param[in] context Pointer to uninitialized edhoc_context
 * @param[in] credentials Pointer to edhoc_credentials struct with fetch/verify
 * callbacks
 * @return Status code indicating success or type of failure during setup
 * @note This function uses edhoc_context_init, so the context needs to be
 * deinitialized with edhoc_context_deinit after use to free resources.
 */
enum com_edhoc_setup_context_status com_edhoc_setup_context(
    struct edhoc_context* context, const struct edhoc_credentials* credentials);

/**
 * @brief Initializes credential key information in EDHOC credentials structure
 * @param[in] credentials Pointer to EDHOC credentials structure
 * @param[in] public_key Public key bytes
 * @param[in] public_key_length Length of public key
 * @param[in] key_id_integer Key identifier as integer
 */
void com_edhoc_initialize_credential_key(struct edhoc_auth_creds* credentials,
                                         const uint8_t* public_key,
                                         size_t public_key_length,
                                         int32_t key_id_integer);

#endif  // EDHOC_COMMON_EDHOC_SETUP_H_
