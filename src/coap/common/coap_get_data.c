/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 06/04/2026
 * @brief Definition for common data retrieval functions for CoAP server and
 * client.
 */

#include "coap/common/coap_get_data.h"

#include <string.h>

#include "coap3/coap_debug.h"
#include "coap3/coap_pdu.h"

static struct com_coap_get_data_result get_data_ok(
    const struct com_readonly_buffer data) {
  return (struct com_coap_get_data_result){
      .status = COM_COAP_GET_DATA_OK,
      .data = data,
  };
}

static struct com_coap_get_data_result get_data_failure(
    const enum com_coap_get_data_status status) {
  return (struct com_coap_get_data_result){.status = status};
}

struct com_coap_get_data_result com_coap_get_data(
    const coap_pdu_t* pdu, struct com_writable_buffer* data_buffer) {
  if (pdu == NULL) {
    coap_log_err("pdu is NULL\n");
    return get_data_failure(COM_COAP_GET_DATA_ERR_PDU);
  }
  if (!com_writable_buffer_is_writable(data_buffer)) {
    coap_log_err("data buffer is not writable\n");
    return get_data_failure(COM_COAP_GET_DATA_ERR_DATA_BUFFER);
  }

  size_t length = 0;
  size_t offset = 0;
  size_t total_length = 0;
  const uint8_t* data = NULL;
  if (!coap_get_data_large(pdu, &length, &data, &offset, &total_length) ||
      data == NULL) {
    coap_log_err("failed to call coap_get_data \n");
    return get_data_failure(COM_COAP_GET_DATA_ERR_NO_PAYLOAD);
  }
  if (offset != 0 || length != total_length) {
    coap_log_err("incomplete body");
    return get_data_failure(COM_COAP_GET_DATA_ERR_INCOMPLETE_BODY);
  }
  if (length > data_buffer->capacity) {
    coap_log_err("data buffer is too small\n");
    return get_data_failure(COM_COAP_GET_DATA_ERR_DATA_BUFFER_TOO_SMALL);
  }
  memcpy(data_buffer->bytes, data, length);
  data_buffer->length = length;
  const struct com_readonly_conversion_result conversion_result =
      com_writable_as_readonly(data_buffer);
  // SHOULD NEVER HAPPEN
  if (conversion_result.status != COM_RDONLY_CONV_OK) {
    return get_data_failure(COM_COAP_GET_DATA_ERR_NO_PAYLOAD);
  }
  return get_data_ok(conversion_result.buffer);
}