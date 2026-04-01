/**
 * @file handle_libedhoc_errors.h
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since Mon 30 Mar
 * @brief Definition of the function to add the edhoc code into the response
 * buffer
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#ifndef EDHOC_SERVER_HANDLE_LIBEDHOC_ERRORS_H_
#define EDHOC_SERVER_HANDLE_LIBEDHOC_ERRORS_H_

#include <edhoc.h>

#include "common/data_models.h"

/**
 * @brief Translates a libedhoc API failure into an EDHOC error message and
 * writes it to the response buffer according to RFC 9528 A.2.3.
 * @param[in] edhoc_api_result Return value from the failing libedhoc API call
 * @param[in] edhoc_ctx EDHOC context carrying the failure details
 * @param[out] response_data Buffer where the EDHOC error message is written
 */
void server_edhoc_add_edhoc_error_to_response(
    int edhoc_api_result, const struct edhoc_context* edhoc_ctx,
    common_response_buffer_t* response_data);

#endif  // EDHOC_SERVER_HANDLE_LIBEDHOC_ERRORS_H_
