/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 01/04/2026
 * @brief Logic for handling EDHOC Message 1 on the server side
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */
#ifndef EDHOC_SERVER_HANDSHAKE_MESSAGE_1_HANDLER_H_
#define EDHOC_SERVER_HANDSHAKE_MESSAGE_1_HANDLER_H_

#include "common/data_models.h"
#include "edhoc/server/handshake/message_1/srv_m1_result.h"

/**
 * @brief Input data required to process EDHOC Message 1.
 */
typedef struct edh_srv_message_1_request {
  /** Incoming Message payload bytes and size. */
  com_readonly_buffer_t payload;

  /** Server credentials for setting up the EDHOC context. */
  const struct edhoc_credentials* credentials;
} edh_srv_message_1_request_t;

/**
 * @brief Handle EDHOC Message 1 and compose Message 2.
 *
 * @param[in] request Session/request metadata for Message 1
 * processing, including credentials.
 * @param[out] response Response buffer metadata for Message 2.
 * @return Struct containing status code and allocated EDHOC context on success,
 * or error code and NULL context on failure.
 * @warning This function dynamically allocates the EDHOC context using
 * calloc(). On success, the caller assumes ownership of this memory and is
 * responsible for freeing it. On failure, the function safely cleans up after
 * itself and returns a NULL pointer.
 */
ehd_srv_message_1_handler_result_t edh_srv_handle_message_1(
    edh_srv_message_1_request_t request, com_writable_buffer_t* response);

const char* edh_srv_handle_message_1_status_code_to_string(
    enum edh_srv_message_1_handler_status status);

#endif  // EDHOC_SERVER_HANDSHAKE_MESSAGE_1_HANDLER_H_
