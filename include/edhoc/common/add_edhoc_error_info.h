/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 30/03/2026
 * @brief Shared EDHOC error-response composition helpers for server handshake
 * handlers.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#ifndef EDHOC_COMMON_ADD_EDHOC_ERROR_INFO_H_
#define EDHOC_COMMON_ADD_EDHOC_ERROR_INFO_H_

#include <edhoc.h>

#include "common/data_models.h"

enum edh_com_set_error_info_status {
  EDH_COM_SET_ERROR_INFO_OK = 0,
  EDH_COM_SET_ERROR_INFO_ERR_NULL_MESSAGE,
  EDH_COM_SET_ERROR_INFO_ERR_NULL_ERROR_INFO,
  EDH_COM_SET_ERROR_INFO_ERR_EMPTY_MESSAGE,
};

/**
 * @brief Populate EDHOC error info with a generic text-string error.
 *
 * @param[in] error_message Generic error text to expose in EDHOC error payload.
 * @param[out] error_info Target struct to initialize.
 * @return Status code indicating success or failure of the operation.
 */
enum edh_com_set_error_info_status edh_com_set_error_info(
    const char* error_message, struct edhoc_error_info* error_info);

enum edh_com_add_edhoc_error_to_response_status {
  EDH_COM_ADD_ERROR_OK = 0,
  EDH_COM_ADD_ERROR_ERR_INVALID_RESPONSE_BUFFER,
  EDH_COM_ADD_ERROR_ERR_COMPOSE
};

/**
 * @brief Compose and write an EDHOC error message payload into the response
 * buffer.
 *
 * Caller is responsible for building error_info according to the operation
 * context (for example Message 1 process vs Message 2 compose) and passing it
 * to this function.
 *
 * @param[in] edhoc_api_result Return value from the failing libedhoc API call.
 * @param[in] error_info Prepared EDHOC error info to embed in the response.
 * @param[out] response_data Buffer where the EDHOC error message is written.
 * @return Status code indicating success or failure of the operation.
 */
enum edh_com_add_edhoc_error_to_response_status
edh_com_add_edhoc_error_to_response(int edhoc_api_result,
                                    const struct edhoc_error_info* error_info,
                                    struct com_writable_buffer* response_data);

#endif  // EDHOC_COMMON_ADD_EDHOC_ERROR_INFO_H_
