/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 06/04/2026
 * @brief Header for common data retrieval functions for CoAP server and client.
 */
#ifndef COAP_COMMON_CP_GET_DATA_H_
#define COAP_COMMON_CP_GET_DATA_H_
#include <coap3/coap.h>

#include "common/data_models.h"

enum cp_com_get_data_status {
  CP_COM_GET_DATA_OK = 0,
  CP_COM_GET_DATA_ERR_INVALID_ARGS,
  CP_COM_GET_DATA_ERR_NO_PAYLOAD,
  CP_COM_GET_DATA_ERR_INCOMPLETE_BODY,
  CP_COM_GET_DATA_ERR_INVALID_BUFFER
};

struct cp_com_get_data_result {
  enum cp_com_get_data_status status;
  struct com_readonly_buffer output;
};

/**
 * @brief Gets the data from the given pdu
 * @param pdu Protocol data unit to get the data from
 * @return Struct containing the status code and the data buffer if the
 * operation was successful, empty buffer and error status code otherwise.
 */
struct cp_com_get_data_result cp_com_get_data(const coap_pdu_t* pdu);

#endif  // COAP_COMMON_CP_GET_DATA_H_