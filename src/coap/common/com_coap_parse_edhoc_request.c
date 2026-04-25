/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 31/03/2026
 * @brief Definitions of functions to process requests
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#include "coap/common/com_coap_parse_edhoc_request.h"

#include "coap/common/com_coap_get_data.h"
#include "coap/common/internal/com_parse_edhoc_request_builders.h"

struct com_coap_parse_edhoc_request_result com_coap_parse_edhoc_request(
    const coap_pdu_t* request,
    const enum config_coap_content_format_edhoc_values expected_format,
    const struct com_writable_buffer data_buffer) {
  if (request == NULL) {
    coap_log_err("request pdu is null\n");
    return srv_coap_internal_parse_edhoc_failure(COM_COAP_EDH_REQ_ERR_PDU);
  }
  if (!com_writable_buffer_is_writable(data_buffer)) {
    coap_log_err("data buffer is not writable\n");
    return srv_coap_internal_parse_edhoc_failure(
        COM_COAP_EDH_REQ_ERR_DATA_BUFFER);
  }

  coap_opt_iterator_t option_iterator = {0};
  const coap_opt_t* content_format_option =
      coap_check_option(request, COAP_OPTION_CONTENT_FORMAT, &option_iterator);
  if (content_format_option == NULL) {
    coap_log_err("missing content format option\n");
    return srv_coap_internal_parse_edhoc_failure(
        COM_COAP_EDH_REQ_ERR_MISSING_FORMAT);
  }
  const uint16_t content_format =
      coap_decode_var_bytes(coap_opt_value(content_format_option),
                            coap_opt_length(content_format_option));
  if (content_format != expected_format) {
    coap_log_err("invalid content format\n");
    return srv_coap_internal_parse_edhoc_failure(
        COM_COAP_EDH_REQ_ERR_UNSUPPORTED_FORMAT);
  }
  const struct com_coap_get_data_result get_data_result =
      com_coap_get_data(request, data_buffer);
  if (get_data_result.status != COM_COAP_GET_DATA_OK) {
    coap_log_err("cannot get request pdu data\n");
    return srv_coap_internal_parse_edhoc_failure(
        COM_COAP_EDH_REQ_ERR_NO_PAYLOAD);
  }
  return srv_coap_internal_parse_edhoc_ok(get_data_result.data);
}

coap_pdu_code_t com_coap_map_parse_result_to_pdu_code(
    const enum com_coap_parse_edhoc_request_status status) {
  switch (status) {
  case COM_COAP_EDH_REQ_OK:
    return COAP_RESPONSE_CODE_CHANGED;

  case COM_COAP_EDH_REQ_ERR_MISSING_FORMAT:
  case COM_COAP_EDH_REQ_ERR_UNSUPPORTED_FORMAT:
  case COM_COAP_EDH_REQ_ERR_NO_PAYLOAD:
    return COAP_RESPONSE_CODE_BAD_REQUEST;

  case COM_COAP_EDH_REQ_ERR_PDU:
  case COM_COAP_EDH_REQ_ERR_DATA_BUFFER:
    return COAP_RESPONSE_CODE_INTERNAL_ERROR;

  default:
    coap_log_warn("Unhandled parse status: %d\n", status);
    return COAP_RESPONSE_CODE_INTERNAL_ERROR;
  }
}
