/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 01/04/2026
 * @brief Message 3 specific EDHOC error-response composition.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#ifndef EDHOC_SERVER_HANDSHAKE_MESSAGE_3_ERRORS_H_
#define EDHOC_SERVER_HANDSHAKE_MESSAGE_3_ERRORS_H_

#include "common/data_models.h"

enum edh_srv_message_3_handler_add_error_status {
  EDH_SRV_MSG3_ADD_ERROR_OK = 0,
  EDH_SRV_MSG3_ADD_ERROR_ERR_SET_ERROR_INFO,
  EDH_SRV_MSG3_ADD_ERROR_ERR_ADD_ERROR_TO_RESPONSE,
};

/**
 * @brief Compose and write a Message 3 operation-specific EDHOC error message
 * into the response buffer.
 *
 * @param[in] edhoc_api_result Return value from the failing libedhoc API call.
 * @param[in] generic_error_message Generic error text for the operation.
 * @param[out] response_data Buffer where the EDHOC error message is written.
 */
enum edh_srv_message_3_handler_add_error_status
edh_srv_message_3_handler_add_error(int edhoc_api_result,
                                    const char* generic_error_message,
                                    struct com_writable_buffer* response_data);

#endif  // EDHOC_SERVER_HANDSHAKE_MESSAGE_3_ERRORS_H_
