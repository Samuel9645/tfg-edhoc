/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 31/03/2026
 * @brief Definitions of functions to process requests
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#include "coap/server/srv_parse_edhoc_request.h"

#include "coap/common/coap_get_data.h"
#include "coap/server/internal/parse_edhoc_result_builders.h"

struct cp_srv_parse_edhoc_request_result cp_srv_parse_edhoc_request(
    const coap_pdu_t* request,
    const enum cp_cfg_content_format_edhoc_values expected_format) {
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
  const struct cp_com_get_data_result get_data_result =
      cp_com_get_data(request);
  if (get_data_result.status != CP_COM_GET_DATA_OK) {
    coap_log_err("cannot get request pdu data\n");
    return cp_srv_internal_parse_edhoc_failure(CP_SRV_EDH_REQ_ERR_NO_PAYLOAD);
  }
  return cp_srv_internal_parse_edhoc_ok(get_data_result.output);
}

const char* cp_srv_parse_edhoc_request_status_to_string(
    const enum cp_srv_parse_edhoc_request_status status) {
  switch (status) {
  case CP_SRV_EDH_REQ_OK:
    return "ok";
  case CP_SRV_EDH_REQ_ERR_INVALID_ARGS:
    return "invalid arguments";
  case CP_SRV_EDH_REQ_ERR_NO_PAYLOAD:
    return "no payload";
  case CP_SRV_EDH_REQ_ERR_UNSUPPORTED_FORMAT:
    return "unsupported format";
  default:
    return "unknown failure";
  }
}
