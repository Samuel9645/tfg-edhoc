/**
 * @file errors.c
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 01/04/2026
 * @brief Message 3 specific EDHOC error-response composition.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#include "edhoc/server/handshake/common/handle_libedhoc_errors.h"
#include "edhoc/server/handshake/message_3/srv_m3_errors.h"

void edh_srv_message_3_handler_add_error(const int edhoc_api_result,
                                     const char* generic_error_message,
                                         com_writable_buffer_t* response_data) {
  struct edhoc_error_info error_info = {0};
  edh_srv_set_error_info(generic_error_message, &error_info);
  edh_srv_add_edhoc_error_to_response(edhoc_api_result, &error_info,
                                      response_data);
}
