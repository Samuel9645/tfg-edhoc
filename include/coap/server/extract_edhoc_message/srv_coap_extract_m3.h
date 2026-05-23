/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 12/05/2026
 * @brief Module to extract and validate EDHOC connection identifiers from
 * Message 3 CoAP request payloads.
 * @see [RFC 9528](https://datatracker.ietf.org/doc/html/rfc9528)
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#ifndef COAP_SERVER_EXTRACT_EDHOC_MESSAGE_SRV_COAP_EXTRACT_M3_H_
#define COAP_SERVER_EXTRACT_EDHOC_MESSAGE_SRV_COAP_EXTRACT_M3_H_

#include <edhoc.h>

#include "common/com_data_models.h"

enum srv_coap_extract_cid_status {
  SRV_COAP_EXTRACT_CID_OK = 0,
  SRV_COAP_EXTRACT_CID_ERR_EMPTY_BUFFER,
  SRV_COAP_EXTRACT_CID_ERR_EXTRACT,
};

struct srv_coap_extract_connection_id_result {
  const enum srv_coap_extract_cid_status status;
  const struct edhoc_connection_id cid;
  const struct com_readonly_buffer message_payload;
};

/**
 * @brief Extracts the connection identifier from an EDHOC Message 3 payload.
 *
 * Decodes and removes the leading connection identifier from the request
 * buffer, leaving the remaining EDHOC message in the result. The operation
 * follows the CoAP EDHOC message encoding defined in RFC 9528 A.2.
 *
 * @param[in] request_buffer Input buffer containing the raw Message 3 payload
 *  with connection ID prefix.
 * @return Result struct with the status of the extraction, the extracted
 * connection ID (if successful), and the remaining message payload after
 * removing the connection ID.
 */
struct srv_coap_extract_connection_id_result srv_coap_extract_connection_id(
    struct com_readonly_buffer request_buffer);

/**
 * @brief Validates that an extracted connection ID matches the EDHOC context.
 *
 * Compares the extracted connection identifier with the private connection ID
 * stored in the EDHOC context to ensure the message is for the expected
 * session.
 *
 * @param[in] extracted_cid The connection identifier extracted from the
 * incoming message.
 * @param[in] edhoc_ctx EDHOC context containing the expected connection ID
 * (private_cid field). Must be non-NULL.
 * @return true if extracted_cid equals the context's private_cid, false
 * otherwise or if edhoc_ctx is NULL.
 */
bool srv_coap_connection_id_is_expected(
    const struct edhoc_connection_id* extracted_cid,
    const struct edhoc_context* edhoc_ctx);

#endif  // COAP_SERVER_EXTRACT_EDHOC_MESSAGE_SRV_COAP_EXTRACT_M3_H_
