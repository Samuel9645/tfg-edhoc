/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 01/04/2026
 * @brief Message 1 processing on the responder side
 * @see [RFC
 * 9528 5.2.3](https://datatracker.ietf.org/doc/html/rfc9528/#name-responder-processing-of-mes)
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#ifndef EDHOC_SERVER_HANDSHAKE_MESSAGE_1_SRV_M1_PROCESS_H_
#define EDHOC_SERVER_HANDSHAKE_MESSAGE_1_SRV_M1_PROCESS_H_
#include <edhoc.h>

#include "common/com_data_models.h"
#include "edhoc/server/handshake/message_1/srv_m1_process_result.h"

/**
 * @brief Handle EDHOC Message 1 and compose Message 2.
 *
 * @param[in] request Raw EDHOC Message 1 request buffer.
 * @param[in] edhoc_context EDHOC context already initialized and configured by
 * the caller.
 * @param[in] error_buffer Metadata of the buffer (pointer and capacity) to
 * write into
 * @return Struct containing status code and, on failure, a view of the written
 * error message.
 */
struct srv_edhoc_message_1_process_result srv_edhoc_process_message_1(
    struct com_readonly_buffer request, struct edhoc_context* edhoc_context,
    struct com_writable_buffer error_buffer);

enum srv_edhoc_cleanup_context_status {
  SRV_EDHOC_CLEANUP_OK = 0,
  SRV_EDHOC_CLEANUP_ERR_NULL_CONTEXT,
  SRV_EDHOC_CLEANUP_ERR_DEINIT
};

/**
 * @brief Deinitializes and frees the memory associated with the given context
 * @param context pointer to the context to clean up.
 * @note On NULL pointer does nothing
 */
enum srv_edhoc_cleanup_context_status srv_edhoc_cleanup_context(
    struct edhoc_context* context);

#endif  // EDHOC_SERVER_HANDSHAKE_MESSAGE_1_SRV_M1_PROCESS_H_
