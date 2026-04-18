/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 18/04/2026
 * @brief Logic for handling EDHOC Message 3 on the server side.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#ifndef EDHOC_SERVER_HANDSHAKE_MESSAGE_3_SRV_M3_RESPONDER_H_
#define EDHOC_SERVER_HANDSHAKE_MESSAGE_3_SRV_M3_RESPONDER_H_

#include "common/com_data_models.h"
#include "edhoc/server/handshake/message_3/srv_m3_process.h"
#include "edhoc/server/handshake/message_3/srv_m3_responder_result.h"

/**
 * @brief Handle EDHOC Message 3 and compose Message 4.
 *
 * @param[in] request Session/request metadata for Message 3 processing.
 * @param[out] response Response buffer metadata for Message 4.
 * @return Struct containing status code and the composed Message 4 buffer on
 * success, or error code and an empty buffer on failure.
 */
struct srv_edhoc_message_3_responder_result srv_edhoc_respond_to_message_3(
    struct srv_edhoc_message_3_request request,
    struct com_writable_buffer* response);

const char* srv_edhoc_handle_message_3_status_code_to_string(
    enum srv_edhoc_message_3_responder_status status);

#endif  // EDHOC_SERVER_HANDSHAKE_MESSAGE_3_SRV_M3_RESPONDER_H_

