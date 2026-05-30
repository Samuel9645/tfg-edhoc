/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 04/04/2026
 * @brief Module to manage EDHOC context creation and cleanup
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 * @see [libedhoc](https://github.com/kamil-kielbasa/libedhoc)
 */
#ifndef EDHOC_COMMON_EDHOC_SETUP_H_
#define EDHOC_COMMON_EDHOC_SETUP_H_

#include <edhoc.h>

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
  COM_EDHOC_SETUP_CTX_ERR_SET_USER_CONTEXT,
  COM_EDHOC_SETUP_CTX_ERR_BIND_CRYPTO,
  COM_EDHOC_SETUP_CTX_ERR_BIND_CREDENTIALS,
};

struct com_edhoc_setup_context_result {
  const enum com_edhoc_setup_context_status status;
};

/**
 * Initializes PSA crypto and configures EDHOC context with cipher suite,
 * methods, and credentials.
 * @param[in] context Pointer to uninitialized edhoc_context
 * @param[in] edhoc_parameters Parameters including credentials and
 * cipher suites data for the EDHOC handshake. The selected cipher suite is
 * added at the end of the suites lists, the last cipher suite is the selected
 * one in both the initiator and responder.
 * @return Struct containing the status of the operation.
 * @note This function uses edhoc_context_init, so the context needs to be
 * deinitialized with edhoc_context_deinit after use to free resources.
 * @note Adding the selected suite at the end does not affect the negotiation
 * with the responder, since the order of the responder suites is irrelevant in
 * that process.
 * @note In order for the credential related functions to access the proper
 * cipher suite import or destroy key methods, a pointer to the edhoc_keys
 * struct is saved in the EDHOC user context, allowing the usage in the
 * cred_edhoc_auth_fetch function
 * @see [RFC
 * 9528 6.3](https://datatracker.ietf.org/doc/html/rfc9528#name-wrong-selected-cipher-suite)
 */
struct com_edhoc_setup_context_result com_edhoc_setup_context(
    struct edhoc_context* context,
    struct com_edhoc_parameters edhoc_parameters);

enum srv_edhoc_cleanup_context_status {
  SRV_EDHOC_CLEANUP_OK = 0,
  SRV_EDHOC_CLEANUP_ERR_NULL_CONTEXT,
  SRV_EDHOC_CLEANUP_ERR_DEINIT
};

/**
 * @brief Deinitializes and frees the memory associated with the given context
 * @param[in] context pointer to the context to clean up.
 * @note On NULL pointer does nothing
 */
enum srv_edhoc_cleanup_context_status srv_edhoc_cleanup_context(
    struct edhoc_context* context);

#endif  // EDHOC_COMMON_EDHOC_SETUP_H_
