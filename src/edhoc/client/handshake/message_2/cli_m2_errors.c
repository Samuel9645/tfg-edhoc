/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 04/04/2026
 * @brief Message 2 specific EDHOC error-response composition for client side.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#include "edhoc/client/handshake/message_2/cli_m2_errors.h"

#include "edhoc/server/handshake/common/handle_libedhoc_errors.h"

void edh_cli_message_2_handler_add_error(const int edhoc_api_result,
                                         const char* generic_error_message,
                                         com_writable_buffer_t* response_data) {
  struct edhoc_error_info error_info = {0};
  edh_srv_set_error_info(generic_error_message, &error_info);
  edh_srv_add_edhoc_error_to_response(edhoc_api_result, &error_info,
                                      response_data);
}
