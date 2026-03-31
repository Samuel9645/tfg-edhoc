#ifndef EDHOC_SERVER_HANDSHAKE_H_
#define EDHOC_SERVER_HANDSHAKE_H_

#include <coap3/coap.h>
#include <edhoc_helpers.h>
#include <stddef.h>
#include <stdint.h>

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
 * @brief Input data required to process EDHOC Message 3.
 */
typedef struct {
  /** Common session/context/response/request metadata. */
  edhoc_server_common_request_data_t base_data;

  /** Pre-extracted Message 3 fields (including inner EDHOC message). */
  struct edhoc_extracted_fields* message_3_extracted_fields;
} edhoc_server_message_3_request_data_t;

/**
 * @brief Error codes for EDHOC server handshake operations.
 */
typedef enum edhoc_server_handshake_status {
  CSH_OK = 0,
  CSH_ERR_INVALID_ARGS,
  CSH_ERR_PAYLOAD_TOO_LARGE,
  CSH_ERR_PREFIX_MISSING,
  CSH_ERR_CALLOC_FAILED,
  CSH_ERR_COAP_SESSION_ALREADY_HAS_DATA,
  CSH_ERR_EDHOC_CONTEXT_SETUP_FAILED,
  CSH_ERR_EDHOC_MESSAGE_1_PROCESS_FAILED,
  CSH_ERR_EDHOC_MESSAGE_2_COMPOSE_FAILED,
} edhoc_server_handshake_status_t;

/**
 * @brief Check whether payload is properly formatted as EDHOC Message 1.
 *
 * @param[in] payload Request payload.
 * @param[in] payload_len Request payload length.
 * @return true if payload matches Message 1 framing, false otherwise.
 */
bool edhoc_server_is_properly_formatted_message_1(const uint8_t* payload,
                                                  size_t payload_len);

/**
 * @brief Check whether payload is properly formatted as EDHOC Message 3.
 *
 * Validates that the payload contains a properly formatted Message 3 with
 * correct connection ID. Extracts connection ID fields for further processing.
 *
 * @param[in] request_payload Request payload.
 * @param[in] request_len Request payload length.
 * @param[in] edhoc_ctx EDHOC context with private connection ID.
 * @param[out] extracted_fields Populated with extracted message fields on
 * success.
 * @return true if payload is valid Message 3, false otherwise.
 */
bool edhoc_server_is_properly_formatted_message_3(
    const uint8_t* request_payload, size_t request_len,
    const struct edhoc_context* edhoc_ctx,
    struct edhoc_extracted_fields* extracted_fields);

/**
 * @brief Strips the CBOR TRUE prefix from the EDHOC Message 1 payload.
 * @param[in,out] payload Pointer to the buffer address; advanced by 1 byte on
 * success.
 * @param[in,out] length Pointer to the buffer length; decremented by 1 on
 * success.
 * @return CSH_OK if prefix was removed, or a CSH_ERR code if invalid/missing.
 */
edhoc_server_handshake_status_t edhoc_server_remove_cbor_true_prefix(
    const uint8_t** payload, size_t* length);

/**
 * @brief Handle EDHOC Message 1 and compose Message 2.
 *
 * @param[in] request_data Session/request metadata for Message 1
 * processing.
 * @param[out] response_data Response buffer metadata for Message 2.
 * @return edhoc_server_handshake_status_t containing the operation result.
 *
 * @note request_data->base_data.edhoc_ctx is a borrowed pointer provided by
 * the dispatcher from CoAP session app-data. It is used for validation and
 * call-scoped protocol operations only; ownership remains in session
 * app-data.
 *
 * @warning Allocates EDHOC context with calloc() and registers as session
 * app-data with free() as destructor. Before process exit, caller must
 * clean the CoAP session/context resources associated with this handshake;
 * use session_resources_t + tfg_common_cleanup_resources() for centralized
 * teardown.
 */
edhoc_server_handshake_status_t edhoc_server_handle_message_1(
    const edhoc_server_common_request_data_t* request_data,
    common_response_buffer_t* response_data);

/**
 * @brief Handle EDHOC Message 3 and compose Message 4.
 *
 * @param[in] request_data Session/request metadata for Message 3 processing.
 * @param[out] response_data Response buffer metadata for Message 4.
 * @return CoAP response code for the operation result.
 *
 * @note request_data->base_data.edhoc_ctx is a borrowed pointer provided by
 * the dispatcher from CoAP session app-data. It is used for validation and
 * call-scoped protocol operations only; ownership remains in session app-data.
 *
 * @warning Before process exit, caller must clean the CoAP session/context
 * resources associated with this handshake; use session_resources_t +
 * tfg_common_cleanup_resources() for centralized teardown.
 */
coap_pdu_code_t edhoc_server_handle_message_3(
    const edhoc_server_message_3_request_data_t* request_data,
    common_response_buffer_t* response_data);

#endif  // EDHOC_SERVER_HANDSHAKE_H_
