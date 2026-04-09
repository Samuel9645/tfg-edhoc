/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 02/04/2026
 * @brief Helper functions for EDHOC server message 3 handler tests.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#ifndef EDHOC_SERVER_HANDSHAKE_MESSAGE_3_HANDLER_HELPERS_H_
#define EDHOC_SERVER_HANDSHAKE_MESSAGE_3_HANDLER_HELPERS_H_

#include <edhoc.h>

#include "edhoc/server/handshake/message_3/srv_m3_handler.h"

enum { TST_EDH_SRV_MESSAGE_3_BUFFER_LENGTH = 256 };

/**
 * @brief Test environment for Message 3 Handler.
 * Contains physical memory buffers and the "View" structs used by the API.
 */
struct tst_edh_srv_message_3_env {
  struct edhoc_context context;
  uint8_t request_payload[TST_EDH_SRV_MESSAGE_3_BUFFER_LENGTH];
  uint8_t response_payload[TST_EDH_SRV_MESSAGE_3_BUFFER_LENGTH];

  struct edh_srv_message_3_request request;
  struct com_writable_buffer response;
};

/**
 * @brief Initialize env buffers and link the 'request' and 'response' views.
 */
void tst_edh_srv_message_3_setup_env(struct tst_edh_srv_message_3_env* env);

#endif  // EDHOC_SERVER_HANDSHAKE_MESSAGE_3_HANDLER_HELPERS_H_
