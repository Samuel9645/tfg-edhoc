/**
 * @file request_data.h
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since Tue 31 Mar
 * @brief Base data models for message 1 and 3 request
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#ifndef EDHOC_SERVER_HANDSHAKE_COMMON_REQUEST_DATA_H_
#define EDHOC_SERVER_HANDSHAKE_COMMON_REQUEST_DATA_H_

#include <coap3/coap.h>

#include "common/data_models.h"

/**
 * @brief Common input data shared by EDHOC Message 1 and Message 3 handlers.
 */
typedef struct {
  /** Session that owns EDHOC app-data for this exchange. */
  coap_session_t* session;

  /** EDHOC context associated with the session before Message 1 handling. */
  struct edhoc_context* edhoc_ctx;

  /** Response PDU used to set response codes on failure paths. */
  coap_pdu_t* response;

  /** Incoming Message payload bytes and size. */
  common_request_payload_t request_data;
} edhoc_server_common_request_data_t;

/**
 * @brief Validate that common EDHOC server request data is properly
 * initialized.
 *
 * Checks that all required pointers and fields are non-NULL and payload has
 * non-zero length.
 *
 * @param[in] request_data Common request data structure.
 * @return true if valid, false if any required field is missing or invalid.
 */
bool edhoc_server_common_request_data_is_valid(
    const edhoc_server_common_request_data_t* request_data);

#endif  // EDHOC_SERVER_HANDSHAKE_COMMON_REQUEST_DATA_H_