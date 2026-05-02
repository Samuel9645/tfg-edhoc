#ifndef EDHOC_COMMON_EDHOC_SETUP_H_
#define EDHOC_COMMON_EDHOC_SETUP_H_

#include <edhoc.h>

#include "common/com_data_models.h"
#include "edhoc/common/com_edhoc_parameters.h"

enum com_edhoc_setup_context_status {
  COM_EDHOC_SETUP_CTX_OK = 0,
  COM_EDHOC_SETUP_CTX_ERR_INVALID_ERROR_BUFFER,
  COM_EDHOC_SETUP_CTX_ERR_INVALID_EDHOC_PARAMETERS,
  COM_EDHOC_SETUP_CTX_ERR_PSA_INIT,
  COM_EDHOC_SETUP_CTX_ERR_CONTEXT_INIT,
  COM_EDHOC_SETUP_CTX_ERR_SET_METHODS,
  COM_EDHOC_SETUP_CTX_ERR_SET_CIPHER_SUITES,
  COM_EDHOC_SETUP_CTX_ERR_SET_CONNECTION_ID,
  COM_EDHOC_SETUP_CTX_ERR_BIND_KEYS,
  COM_EDHOC_SETUP_CTX_ERR_BIND_CRYPTO,
  COM_EDHOC_SETUP_CTX_ERR_BIND_CREDENTIALS,
};

struct com_edhoc_setup_context_result {
  const enum com_edhoc_setup_context_status status;
  const struct com_readonly_buffer error_buffer;
};

/**
 * Initializes PSA crypto and configures EDHOC context with cipher suite,
 * methods, and credentials.
 * @param[in] context Pointer to uninitialized edhoc_context
 * @param[in] edhoc_parameters Parameters including credentials and supported
 * cipher suites for the EDHOC handshake.
 * @param[in] error_buffer Writable buffer to write error messages in case of
 * failure.
 * @return Struct containing the status of the operation and a view of the
 * filled buffer on failure, empty view with success code on success
 * @note This function uses edhoc_context_init, so the context needs to be
 * deinitialized with edhoc_context_deinit after use to free resources.
 */
struct com_edhoc_setup_context_result com_edhoc_setup_context(
    struct edhoc_context* context, struct srv_edhoc_parameters edhoc_parameters,
    struct com_writable_buffer error_buffer);

#endif  // EDHOC_COMMON_EDHOC_SETUP_H_
