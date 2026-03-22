#include "edhoc/client/client_flow.h"

#include <edhoc_helpers.h>
#include <edhoc_values.h>
#include <string.h>

#include "coap/common/config.h"
#include "edhoc/common/config.h"
#include "edhoc/common/setup.h"
#include "edhoc/credentials/authentication.h"
#include "edhoc/credentials/client_private_key.h"
#include "edhoc/credentials/public_data.h"

static int client_credential_fetch(void* user_context,
                                   struct edhoc_auth_creds* credentials) {
  return credential_fetch(user_context, credentials, CLIENT_PUBLIC_KEY,
                          ARRAY_SIZE(CLIENT_PUBLIC_KEY), CLIENT_PRIVATE_KEY,
                          ARRAY_SIZE(CLIENT_PRIVATE_KEY), CLIENT_KID);
}

static int client_credential_verify(void* user_context,
                                    struct edhoc_auth_creds* credentials,
                                    const uint8_t** public_key_reference,
                                    size_t* public_key_length) {
  return credential_verify(user_context, credentials, SERVER_KID,
                           SERVER_PUBLIC_KEY, ARRAY_SIZE(SERVER_PUBLIC_KEY),
                           public_key_reference, public_key_length);
}

client_edhoc_flow_status_t client_edhoc_flow_init(client_edhoc_flow_t* flow) {
  if (!flow) {
    return CLIENT_EDHOC_FLOW_INVALID_ARGUMENT;
  }

  memset(flow, 0, sizeof(*flow));

  const struct edhoc_credentials credentials = {
      .fetch = client_credential_fetch,
      .verify = client_credential_verify,
  };

  if (edhoc_setup_context(&flow->context, &credentials) != EDHOC_SUCCESS) {
    return CLIENT_EDHOC_FLOW_CONTEXT_SETUP_FAILED;
  }

  flow->initialized = true;

  return CLIENT_EDHOC_FLOW_SUCCESS;
}

client_edhoc_flow_status_t client_edhoc_flow_compose_message_1(
    client_edhoc_flow_t* flow, size_t payload_capacity, uint8_t* payload,
    size_t* payload_len) {
  if (!flow || !payload || !payload_len || payload_capacity <= 1) {
    return CLIENT_EDHOC_FLOW_INVALID_ARGUMENT;
  }

  payload[0] = CBOR_TRUE;
  size_t message_len = 0;
  if (edhoc_message_1_compose(&flow->context, &payload[1], payload_capacity - 1,
                              &message_len) != EDHOC_SUCCESS) {
    return CLIENT_EDHOC_FLOW_MESSAGE_1_COMPOSE_FAILED;
  }

  *payload_len = message_len + 1;
  return CLIENT_EDHOC_FLOW_SUCCESS;
}

client_edhoc_flow_status_t client_edhoc_flow_process_message_2(
    client_edhoc_flow_t* flow, const uint8_t* payload, size_t payload_len) {
  if (!flow || !payload || payload_len == 0) {
    return CLIENT_EDHOC_FLOW_INVALID_ARGUMENT;
  }

  if (edhoc_message_2_process(&flow->context, payload, payload_len) !=
      EDHOC_SUCCESS) {
    return CLIENT_EDHOC_FLOW_MESSAGE_2_PROCESS_FAILED;
  }

  return CLIENT_EDHOC_FLOW_SUCCESS;
}

client_edhoc_flow_status_t client_edhoc_flow_compose_message_3(
    client_edhoc_flow_t* flow, size_t payload_capacity, uint8_t* payload,
    size_t* payload_len) {
  if (!flow || !payload || !payload_len || payload_capacity == 0) {
    return CLIENT_EDHOC_FLOW_INVALID_ARGUMENT;
  }

  struct edhoc_prepended_fields prepended_fields = {
      .buffer = payload,
      .buffer_size = payload_capacity,
      .edhoc_message_ptr = payload,
      .edhoc_message_size = payload_capacity,
  };

  if (edhoc_prepend_connection_id(&prepended_fields,
                                  &flow->context.private_peer_cid) !=
      EDHOC_SUCCESS) {
    return CLIENT_EDHOC_FLOW_CONNECTION_ID_PREPEND_FAILED;
  }

  size_t message3_len = 0;
  if (edhoc_message_3_compose(&flow->context,
                              prepended_fields.edhoc_message_ptr,
                              prepended_fields.edhoc_message_size,
                              &message3_len) != EDHOC_SUCCESS) {
    return CLIENT_EDHOC_FLOW_MESSAGE_3_COMPOSE_FAILED;
  }

  prepended_fields.edhoc_message_size = message3_len;
  if (edhoc_prepend_recalculate_size(&prepended_fields) != EDHOC_SUCCESS) {
    return CLIENT_EDHOC_FLOW_PREPEND_RECALCULATION_FAILED;
  }

  *payload_len = prepended_fields.buffer_size;
  return CLIENT_EDHOC_FLOW_SUCCESS;
}

client_edhoc_flow_status_t client_edhoc_flow_process_message_4(
    client_edhoc_flow_t* flow, const uint8_t* payload, size_t payload_len) {
  if (!flow || !payload || payload_len == 0) {
    return CLIENT_EDHOC_FLOW_INVALID_ARGUMENT;
  }

  if (edhoc_message_4_process(&flow->context, payload, payload_len) !=
      EDHOC_SUCCESS) {
    return CLIENT_EDHOC_FLOW_MESSAGE_4_PROCESS_FAILED;
  }

  return CLIENT_EDHOC_FLOW_SUCCESS;
}

void client_edhoc_flow_deinit(client_edhoc_flow_t* flow) {
  if (!flow || !flow->initialized) {
    return;
  }

  (void)edhoc_context_deinit(&flow->context);
  memset(flow, 0, sizeof(*flow));
}
