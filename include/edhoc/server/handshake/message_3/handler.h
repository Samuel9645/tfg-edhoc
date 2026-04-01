/**
 * @file handler.h
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 01/04/2026
 * @brief Holds the core logic for the EDHOC handshake
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#ifndef EDHOC_SERVER_HANDSHAKE_MESSAGE_3_HANDLER_H_
#define EDHOC_SERVER_HANDSHAKE_MESSAGE_3_HANDLER_H_

#include <edhoc_helpers.h>
#include <stddef.h>

#include "common/data_models.h"
#include "edhoc/server/handshake/common/request_data.h"
#include "edhoc/server/handshake/message_3/result.h"

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
 * @brief Check whether payload is properly formatted as EDHOC Message 3 and
 * then extract the fields if so.
 * @param[in] request_payload Request payload.
 * @param[in] request_len Request payload length.
 * @param[in] edhoc_ctx EDHOC context with private connection ID.
 * @param[out] extracted_fields Populated with extracted message fields on
 * success.
 * @return true if payload is valid Message 3, false otherwise.
 */
bool edhoc_server_extract_if_properly_formatted_message_3(
    const uint8_t* request_payload, size_t request_len,
    const struct edhoc_context* edhoc_ctx,
    struct edhoc_extracted_fields* extracted_fields);

/**
 * @brief Handle EDHOC Message 3 and compose Message 4.
 *
 * @param[in] request_data Session/request metadata for Message 3 processing.
 * @param[out] response_data Response buffer metadata for Message 4.
 * @return Struct containing Message 3 operation status.
 *
 * @note request_data->base_data.edhoc_ctx is a borrowed pointer provided by
 * the dispatcher from CoAP session app-data. It is used for validation and
 * call-scoped protocol operations only; ownership remains in session app-data.
 *
 * @warning Before process exit, caller must clean the CoAP session/context
 * resources associated with this handshake; use session_resources_t +
 * tfg_common_cleanup_resources() for centralized teardown.
 */
edhoc_server_message_3_result_t edhoc_server_handle_message_3(
    const edhoc_server_message_3_request_data_t* request_data,
    common_response_buffer_t* response_data);

#endif  // EDHOC_SERVER_HANDSHAKE_MESSAGE_3_HANDLER_H_
