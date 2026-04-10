/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 31/03/2026
 * @brief Contains the functions related to CoAP request
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#ifndef COAP_SERVER_SRV_PARSE_EDHOC_REQUEST_H_
#define COAP_SERVER_SRV_PARSE_EDHOC_REQUEST_H_

#include <coap3/coap.h>

#include "coap/coap_config.h"
#include "common/com_data_models.h"

enum srv_coap_parse_edhoc_request_status {
  SRV_COAP_EDH_REQ_OK = 0,
  SRV_COAP_EDH_REQ_ERR_PDU,
  SRV_COAP_EDH_REQ_ERR_DATA_BUFFER,
  SRV_COAP_EDH_REQ_ERR_MISSING_FORMAT,
  SRV_COAP_EDH_REQ_ERR_UNSUPPORTED_FORMAT,
  SRV_COAP_EDH_REQ_ERR_NO_PAYLOAD,
};

struct srv_coap_parse_edhoc_request_result {
  enum srv_coap_parse_edhoc_request_status status;
  const struct com_readonly_buffer parsed_request;
};

/**
 * @brief Validate EDHOC CoAP request content format and extract payload.
 * @param[in] request Incoming CoAP request.
 * @param[in] expected_format Expected content format value.
 * @param[out] data_buffer View of the buffer to write the data into
 * @return Struct containing a status code and a view of the parsed request
 */
struct srv_coap_parse_edhoc_request_result srv_coap_parse_edhoc_request(
    const coap_pdu_t* request,
    enum config_coap_content_format_edhoc_values expected_format,
    struct com_writable_buffer* data_buffer);

coap_pdu_code_t srv_coap_map_parse_result_to_pdu_code(
    enum srv_coap_parse_edhoc_request_status status);

#endif  // COAP_SERVER_SRV_PARSE_EDHOC_REQUEST_H_