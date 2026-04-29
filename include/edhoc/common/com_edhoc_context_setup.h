#ifndef EDHOC_COMMON_EDHOC_SETUP_H_
#define EDHOC_COMMON_EDHOC_SETUP_H_

#include <edhoc.h>

#include "edhoc/common/com_edhoc_parameters.h"

enum com_edhoc_setup_context_status {
  COM_EDHOC_SETUP_CTX_OK = 0,
  COM_EDHOC_SETUP_CTX_ERR_PSA_INIT,
  COM_EDHOC_SETUP_CTX_ERR_CONTEXT_INIT,
  COM_EDHOC_SETUP_CTX_ERR_INVALID_SUPPORTED_SUITES,
  COM_EDHOC_SETUP_CTX_ERR_INVALID_METHODS,
  COM_EDHOC_SETUP_CTX_ERR_NULL_CREDENTIALS,
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
 * @param[in] edhoc_parameters Parameters including credentials and supported
 * cipher suites for the EDHOC handshake.
 * @return Status code indicating success or type of failure during setup
 * @note This function uses edhoc_context_init, so the context needs to be
 * deinitialized with edhoc_context_deinit after use to free resources.
 */
enum com_edhoc_setup_context_status com_edhoc_setup_context(
    struct edhoc_context* context,
    struct srv_edhoc_parameters edhoc_parameters);

#endif  // EDHOC_COMMON_EDHOC_SETUP_H_
