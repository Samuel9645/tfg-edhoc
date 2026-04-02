/**
 * @file helpers.h
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 02/04/2026
 * @brief Helper functions for EDHOC server message 3 handler tests.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#ifndef EDHOC_SERVER_HANDSHAKE_MESSAGE_3_HANDLER_HELPERS_H_
#define EDHOC_SERVER_HANDSHAKE_MESSAGE_3_HANDLER_HELPERS_H_

#include "edhoc/server/handshake/message_3/handler.h"

enum { TST_EDH_SRV_MESSAGE_3_BUFFER_LENGTH = 256 };

typedef struct {
  int context_dummy;
  struct edhoc_extracted_fields extracted_fields;
  uint8_t request_payload[TST_EDH_SRV_MESSAGE_3_BUFFER_LENGTH];
  uint8_t response_payload[TST_EDH_SRV_MESSAGE_3_BUFFER_LENGTH];
  size_t response_written_len;
  edh_srv_message_3_request_t request;
  com_response_buffer_t response;
} tst_edh_srv_message_3_env_t;

void tst_edh_srv_message_3_setup_env(tst_edh_srv_message_3_env_t* env);

void tst_edh_srv_message_3_reset_response(com_response_buffer_t* response);

void tst_edh_srv_message_3_assert_response_clean(
    const com_response_buffer_t* response);

#endif  // EDHOC_SERVER_HANDSHAKE_MESSAGE_3_HANDLER_HELPERS_H_
