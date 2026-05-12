/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 18/04/2026
 * @brief Message 3 processing on the responder side.
 * @see [RFC
 * 9528 5.4.3](https://datatracker.ietf.org/doc/html/rfc9528/#name-responder-processing-of-mess)
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#ifndef EDHOC_SERVER_HANDSHAKE_MESSAGE_3_SRV_M3_PROCESS_H_
#define EDHOC_SERVER_HANDSHAKE_MESSAGE_3_SRV_M3_PROCESS_H_

#include "common/com_data_models.h"
#include "edhoc/server/handshake/message_3/srv_m3_process_result.h"

struct srv_edhoc_message_3_request {
  struct edhoc_context* edhoc_context;
  const struct com_readonly_buffer message_3;
};

/**
 * @brief Handle EDHOC Message 3 processing and write an error payload when it
 * fails.
 *
 * @param[in] request Session/request metadata for Message 3 processing.
 * @param[in] error_buffer Metadata of the buffer (pointer and capacity) to
 * write into
 * @return Struct containing status code and written error buffer on failure.
 */
struct srv_edhoc_message_3_process_result srv_edhoc_process_message_3(
    struct srv_edhoc_message_3_request request,
    struct com_writable_buffer error_buffer);
#endif  // EDHOC_SERVER_HANDSHAKE_MESSAGE_3_SRV_M3_PROCESS_H_
