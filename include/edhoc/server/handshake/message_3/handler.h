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

#include "common/data_models.h"
#include "edhoc/server/handshake/message_3/result.h"

/**
 * @brief Input data required to process EDHOC Message 3.
 */
typedef struct edh_srv_message_3_request {
  /** EDHOC context associated with the session */
  struct edhoc_context* edhoc_ctx;

  /** Pre-extracted Message 3 fields (including inner EDHOC message). */
  struct edhoc_extracted_fields* message_3_extracted_fields;
} edh_srv_message_3_request_t;

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
 * resources associated with this handshake; use com_session_resources_t +
 * com_cleanup_resources() for centralized teardown.
 */
edh_message_3_handler_status_t edh_srv_handle_message_3(
    const edh_srv_message_3_request_t* request_data,
    com_response_buffer_t* response_data);

#endif  // EDHOC_SERVER_HANDSHAKE_MESSAGE_3_HANDLER_H_
