/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 01/04/2026
 * @brief Holds the core logic for the EDHOC handshake
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#ifndef EDHOC_SERVER_HANDSHAKE_MESSAGE_3_SRV_M3_HANDLER_H_
#define EDHOC_SERVER_HANDSHAKE_MESSAGE_3_SRV_M3_HANDLER_H_

#include "common/com_data_models.h"
#include "edhoc/server/handshake/message_3/srv_m3_result.h"

struct edh_srv_message_3_request {
  struct edhoc_context* edhoc_context;
  struct com_readonly_buffer parsed_message_3;
};

/**
 * @brief Handle EDHOC Message 3 and compose Message 4.
 * @param[in] request Session/request metadata for Message 3 processing.
 * @param[out] response_buffer Response buffer metadata for Message 4.
 * @return Message 3 operation status.
 *
 * @note request_data->base_data.edhoc_ctx is a borrowed pointer provided by
 * the dispatcher.
 */
enum edh_srv_message_3_handler_status edh_srv_handle_message_3(
    struct edh_srv_message_3_request request,
    struct com_writable_buffer* response_buffer);

const char* edh_srv_handle_message_3_status_code_to_string(
    enum edh_srv_message_3_handler_status status);

#endif  // EDHOC_SERVER_HANDSHAKE_MESSAGE_3_SRV_M3_HANDLER_H_
