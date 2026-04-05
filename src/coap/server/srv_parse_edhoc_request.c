/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 31/03/2026
 * @brief Definitions of functions to process requests
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#include "coap/server/srv_parse_edhoc_request.h"

#include "internal/parse_edhoc_result_builders.h"

cp_srv_parse_edhoc_request_result_t cp_srv_parse_edhoc_request(
    const coap_pdu_t* request,
    const cp_cfg_content_format_edhoc_values_t expected_format) {
  if (request == NULL) {
    coap_log_err("request pdu is null\n");
    return cp_srv_internal_parse_edhoc_failure(CP_SRV_EDH_REQ_ERR_INVALID_ARGS);
  }

  coap_opt_iterator_t option_iterator = {0};
  coap_opt_t* content_format_option =
      coap_check_option(request, COAP_OPTION_CONTENT_FORMAT, &option_iterator);
  if (!content_format_option) {
    coap_log_err("missing content format option\n");
    return cp_srv_internal_parse_edhoc_failure(
        CP_SRV_EDH_REQ_ERR_MISSING_FORMAT);
  }
  const uint16_t content_format =
      coap_decode_var_bytes(coap_opt_value(content_format_option),
                            coap_opt_length(content_format_option));
  if (content_format != expected_format) {
    coap_log_err("invalid content format\n");
    return cp_srv_internal_parse_edhoc_failure(
        CP_SRV_EDH_REQ_ERR_UNSUPPORTED_FORMAT);
  }
  size_t len = 0;
  const uint8_t* data = NULL;
  if (!coap_get_data(request, &len, &data) || data == NULL) {
    coap_log_err("cannot get request pdu data\n");
    return cp_srv_internal_parse_edhoc_failure(CP_SRV_EDH_REQ_ERR_NO_PAYLOAD);
  }
  return cp_srv_internal_parse_edhoc_ok((com_readonly_buffer_t){
      .bytes = data,
      .length = len,
  });
}
