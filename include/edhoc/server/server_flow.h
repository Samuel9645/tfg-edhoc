#ifndef EDHOC_SERVER_SERVER_FLOW_H_
#define EDHOC_SERVER_SERVER_FLOW_H_

#include <coap3/coap.h>
#include <edhoc_helpers.h>
#include <stddef.h>
#include <stdint.h>

#include "coap/common/data_models.h"

/**
 * @brief Input data required to process EDHOC Message 1.
 */
typedef struct {
  /** Session that owns EDHOC app-data for this exchange. */
  coap_session_t* session;

  /** Response PDU used to set response codes on failure paths. */
  coap_pdu_t* response;

  /** Incoming Message 1 payload bytes and size. */
  coap_request_data_t request_data;
} server_edhoc_message_1_request_data_t;

/**
 * @brief Input data required to process EDHOC Message 3.
 */
typedef struct {
  /** Session that owns EDHOC app-data for this exchange. */
  coap_session_t* session;

  /** Response PDU used to set response codes on failure paths. */
  coap_pdu_t* response;

  /** Incoming Message 3 payload bytes and size. */
  coap_request_data_t request_data;

  /** Pre-extracted Message 3 fields (including inner EDHOC message). */
  struct edhoc_extracted_fields* extracted_fields;
} server_edhoc_message_3_request_data_t;

/**
 * @brief Handle EDHOC Message 1 and compose Message 2.
 *
 * @param[in] request_data Session/request metadata for Message 1 processing.
 * @param[out] response_data Response buffer metadata for Message 2.
 * @return CoAP response code for the operation result.
 *
 * @warning Allocates EDHOC context with calloc() and registers as session
 * app-data with free() as destructor. Caller must ensure the session is
 * properly cleaned up before application exit.
 */
coap_pdu_code_t server_edhoc_handle_message_1(
    const server_edhoc_message_1_request_data_t* request_data,
    const coap_response_data_t* response_data);

/**
 * @brief Handle EDHOC Message 3 and compose Message 4.
 *
 * @param[in] request_data Session/request metadata for Message 3 processing.
 * @param[out] response_data Response buffer metadata for Message 4.
 * @return CoAP response code for the operation result.
 */
coap_pdu_code_t server_edhoc_handle_message_3(
    const server_edhoc_message_3_request_data_t* request_data,
    const coap_response_data_t* response_data);

#endif  // EDHOC_SERVER_SERVER_FLOW_H_
