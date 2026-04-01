/**
 * @file request_data.c
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 01/04/2026
 * @brief Validation helpers for shared EDHOC server request data.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */
#include "edhoc/server/handshake/common/request_data.h"

bool edh_srv_hnd_com_request_data_is_valid(
    const edh_srv_hnd_com_request_t* request_data) {
  return request_data != NULL && request_data->session != NULL &&
         request_data->response != NULL &&
         request_data->request_data.payload != NULL &&
         request_data->request_data.payload_length > 0;
}