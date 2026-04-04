/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 30/03/2026
 * @brief Shared EDHOC error-response composition helpers for server handshake
 * handlers.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#ifndef EDHOC_SERVER_HANDSHAKE_COMMON_HANDLE_LIBEDHOC_ERRORS_H_
#define EDHOC_SERVER_HANDSHAKE_COMMON_HANDLE_LIBEDHOC_ERRORS_H_

#include <edhoc.h>

#include "common/data_models.h"

/**
 * @brief Populate EDHOC error info with a generic text-string error.
 *
 * @param[in] error_message Generic error text to expose in EDHOC error payload.
 * @param[out] error_info Target struct to initialize.
 */
void edh_srv_set_error_info(const char* error_message,
                            struct edhoc_error_info* error_info);

/**
 * @brief Compose and write an EDHOC error message payload into the response
 * buffer.
 *
 * Caller is responsible for building @p error_info according to the operation
 * context (for example Message 1 process vs Message 2 compose) and passing it
 * to this function.
 *
 * @param[in] edhoc_api_result Return value from the failing libedhoc API call.
 * @param[in] error_info Prepared EDHOC error info to embed in the response.
 * @param[out] response_data Buffer where the EDHOC error message is written.
 */
void edh_srv_add_edhoc_error_to_response(
    int edhoc_api_result, const struct edhoc_error_info* error_info,
    com_writable_buffer_t* response_data);

#endif  // EDHOC_SERVER_HANDSHAKE_COMMON_HANDLE_LIBEDHOC_ERRORS_H_
