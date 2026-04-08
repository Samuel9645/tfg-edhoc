/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 30/03/2026
 * @brief Helper functions for EDHOC server handshake unit tests.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#ifndef EDHOC_SERVER_HANDSHAKE_HELPERS_H_
#define EDHOC_SERVER_HANDSHAKE_HELPERS_H_

#include "edhoc/server/handshake/message_1/srv_m1_handler.h"

enum { TST_EDH_SRV_HND_BUF_LEN = 256 };

struct tst_message_1_handler_env {
  uint8_t request_payload[TST_EDH_SRV_HND_BUF_LEN];
  uint8_t response_payload[TST_EDH_SRV_HND_BUF_LEN];
  size_t response_written_len;
  struct edh_srv_message_1_request valid_request;
  struct com_writable_buffer response;
};

void tst_edh_clear_message_1_setup_env(struct tst_message_1_handler_env* env);

#endif  // EDHOC_SERVER_HANDSHAKE_HELPERS_H_