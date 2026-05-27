/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 06/04/2026
 * @brief Header for common data retrieval functions for CoAP server and client.
 */
#ifndef COAP_COMMON_COAP_GET_DATA_H_
#define COAP_COMMON_COAP_GET_DATA_H_
#include <coap3/coap.h>

#include "common/com_data_models.h"

enum com_coap_get_data_status {
  COM_COAP_GET_DATA_OK = 0,
  COM_COAP_GET_DATA_ERR_PDU,
  COM_COAP_GET_DATA_ERR_DATA_BUFFER,
  COM_COAP_GET_DATA_ERR_NO_PAYLOAD,
  COM_COAP_GET_DATA_ERR_INCOMPLETE_BODY,
  COM_COAP_GET_DATA_ERR_DATA_BUFFER_TOO_SMALL,
  COM_COAP_GET_DATA_ERR_EMPTY_DATA,
};

struct com_coap_get_data_result {
  const enum com_coap_get_data_status status;
  const struct com_readonly_buffer data;
};

/**
 * @brief Gets the data from the given pdu
 * @param[in] pdu Protocol data unit to get the data from
 * @param[in] data_buffer Metadata of the buffer (pointer and capacity) to write
 * into
 * @return Result struct containing the status of the operation and a view over
 * the data on success, or an empty view with error code on failure.
 */
struct com_coap_get_data_result com_coap_get_data(
    const coap_pdu_t* pdu, struct com_writable_buffer data_buffer);

bool com_pdu_is_empty(const coap_pdu_t* pdu);

#endif  // COAP_COMMON_COAP_GET_DATA_H_