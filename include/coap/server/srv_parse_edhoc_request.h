/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 31/03/2026
 * @brief Contains the functions related to CoAP request
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#ifndef COAP_SERVER_REQUEST_H_
#define COAP_SERVER_REQUEST_H_

#include <coap3/coap.h>

#include "coap/config.h"
#include "common/data_models.h"

enum cp_srv_parse_edhoc_request_status {
  CP_SRV_EDH_REQ_OK = 0,
  CP_SRV_EDH_REQ_ERR_INVALID_ARGS,
  CP_SRV_EDH_REQ_ERR_MISSING_FORMAT,
  CP_SRV_EDH_REQ_ERR_UNSUPPORTED_FORMAT,
  CP_SRV_EDH_REQ_ERR_NO_PAYLOAD,
  CP_SRV_EDH_REQ_ERR_MALFORMED_PDU
};

struct cp_srv_parse_edhoc_request_result {
  enum cp_srv_parse_edhoc_request_status status;
  const struct com_readonly_buffer parsed_request;
};

/**
 * @brief Validate EDHOC CoAP request content format and extract payload.
 * @param[in] request Incoming CoAP request.
 * @param[in] expected_format Expected content format value.
 * @return Struct containing the status and the parsed response on success,
 * empty response on failure
 */
struct cp_srv_parse_edhoc_request_result cp_srv_parse_edhoc_request(
    const coap_pdu_t* request,
    enum cp_cfg_content_format_edhoc_values expected_format);

/**
 * @brief Convert a parsing status code into a human-readable string.
 * @param status status of the parsing operation
 * @return human-readable string describing the parsing status, useful for
 * logging and debugging.
 */
const char* cp_srv_parse_edhoc_request_status_to_string(
    enum cp_srv_parse_edhoc_request_status status);

#endif  // COAP_SERVER_REQUEST_H_