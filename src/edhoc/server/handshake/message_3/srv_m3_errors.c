/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 01/04/2026
 * @brief Message 3 specific EDHOC error-response composition.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#include "edhoc/server/handshake/message_3/srv_m3_errors.h"

#include "edhoc/common/add_edhoc_error_info.h"

enum edh_srv_message_3_handler_add_error_status
edh_srv_message_3_handler_add_error(const int edhoc_api_result,
                                    const char* generic_error_message,
                                    struct com_writable_buffer* response_data) {
  struct edhoc_error_info error_info = {0};
  if (edh_com_set_error_info(generic_error_message, &error_info) !=
      EDH_COM_SET_ERROR_INFO_OK) {
    return EDH_SRV_MSG3_ADD_ERROR_ERR_SET_ERROR_INFO;
  }
  if (edh_com_add_edhoc_error_to_response(edhoc_api_result, &error_info,
                                          response_data) !=
      EDH_COM_ADD_ERROR_OK) {
    return EDH_SRV_MSG3_ADD_ERROR_ERR_ADD_ERROR_TO_RESPONSE;
  }
  return EDH_SRV_MSG3_ADD_ERROR_OK;
}
