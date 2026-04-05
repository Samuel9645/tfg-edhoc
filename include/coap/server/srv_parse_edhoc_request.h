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

/**
 * @brief Detailed status for server-side request processing.
 */
typedef enum cp_srv_parse_edhoc_request_status {
  CP_SRV_EDH_REQ_OK = 0,
  CP_SRV_EDH_REQ_ERR_INVALID_ARGS,
  CP_SRV_EDH_REQ_ERR_MISSING_FORMAT,
  CP_SRV_EDH_REQ_ERR_UNSUPPORTED_FORMAT,
  CP_SRV_EDH_REQ_ERR_NO_PAYLOAD,
  CP_SRV_EDH_REQ_ERR_MALFORMED_PDU
} cp_srv_parse_edhoc_request_status_t;

typedef struct cp_srv_parse_edhoc_request_result {
  cp_srv_parse_edhoc_request_status_t status;
  const com_readonly_buffer_t parsed_request;
} cp_srv_parse_edhoc_request_result_t;

/**
 * @brief Validate EDHOC CoAP request content format and extract payload.
 * @param[in] request Incoming CoAP request.
 * @param[in] expected_format Expected content format value.
 * @return Struct containing the status and the parsed response on success,
 * empty response on failure
 */
cp_srv_parse_edhoc_request_result_t cp_srv_parse_edhoc_request(
    const coap_pdu_t* request,
    cp_cfg_content_format_edhoc_values_t expected_format);

/**
 * @brief Convert a parsing status code into a human-readable string.
 * @param status status of the parsing operation
 * @return human-readable string describing the parsing status, useful for
 * logging and debugging.
 */
const char* cp_srv_parse_edhoc_request_status_to_string(
    cp_srv_parse_edhoc_request_status_t status);

#endif  // COAP_SERVER_REQUEST_H_