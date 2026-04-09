/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 06/04/2026
 * @brief Definition for common data retrieval functions for CoAP server and
 * client.
 */

#include "coap/common/coap_get_data.h"
#include "coap3/coap_debug.h"
#include "coap3/coap_pdu.h"

static struct cp_com_get_data_result get_data_ok(
    const struct com_readonly_buffer buffer) {
  return (struct cp_com_get_data_result){.status = CP_COM_GET_DATA_OK,
                                         .output = buffer};
}

static struct cp_com_get_data_result get_data_failure(
    const enum cp_com_get_data_status error_status) {
  return (struct cp_com_get_data_result){.status = error_status};
}

struct cp_com_get_data_result cp_com_get_data(const coap_pdu_t* pdu) {
  if (pdu == NULL) {
    coap_log_err("invalid arguments to cp_com_get_data\n");
    return get_data_failure(CP_COM_GET_DATA_ERR_INVALID_ARGS);
  }

  size_t length = 0;
  size_t offset = 0;
  size_t total_length = 0;
  const uint8_t* data = NULL;
  if (!coap_get_data_large(pdu, &length, &data, &offset, &total_length) ||
      data == NULL) {
    coap_log_err("failed to call coap_get_data \n");
    return get_data_failure(CP_COM_GET_DATA_ERR_NO_PAYLOAD);
  }
  if (offset != 0 || length != total_length) {
    coap_log_err("incomplete body");
    return get_data_failure(CP_COM_GET_DATA_ERR_INCOMPLETE_BODY);
  }
  const struct com_readonly_buffer buffer = {.bytes = data, .length = length};
  if (!com_readonly_buffer_is_valid(buffer)) {
    coap_log_err("invalid buffer retrieved from coap_get_data\n");
    return get_data_failure(CP_COM_GET_DATA_ERR_INVALID_BUFFER);
  }
  return get_data_ok(buffer);
}