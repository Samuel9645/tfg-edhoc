/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 01/04/2026
 * @brief Holds the core logic for the EDHOC handshake
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#ifndef EDHOC_SERVER_HANDSHAKE_MESSAGE_3_HANDLER_H_
#define EDHOC_SERVER_HANDSHAKE_MESSAGE_3_HANDLER_H_

#include <edhoc_helpers.h>

#include "common/data_models.h"
#include "edhoc/server/handshake/message_3/srv_m3_result.h"

/**
 * @brief Input data required to process EDHOC Message 3.
 */
typedef struct edh_srv_message_3_request {
  struct edhoc_context* edhoc_ctx;
  com_readonly_buffer_t parsed_message_3;
} edh_srv_message_3_request_t;

/**
 * @brief Handle EDHOC Message 3 and compose Message 4.
 * @param[in] request Session/request metadata for Message 3 processing.
 * @param[out] response_buffer Response buffer metadata for Message 4.
 * @return Message 3 operation status.
 *
 * @note request_data->base_data.edhoc_ctx is a borrowed pointer provided by
 * the dispatcher.
 */
edh_srv_message_3_handler_status_t edh_srv_handle_message_3(
    edh_srv_message_3_request_t request,
    com_writable_buffer_t* response_buffer);

const char* edh_srv_handle_message_3_status_code_to_string(
    edh_srv_message_3_handler_status_t status);

#endif  // EDHOC_SERVER_HANDSHAKE_MESSAGE_3_HANDLER_H_
