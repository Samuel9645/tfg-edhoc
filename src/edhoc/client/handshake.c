#include "edhoc/client/handshake.h"

#include <edhoc_helpers.h>
#include <edhoc_values.h>
#include <string.h>

#include "edhoc/common/constants.h"
#include "edhoc/common/setup.h"
#include "edhoc/credentials/authentication.h"
#include "edhoc/credentials/client_private_key.h"
#include "edhoc/credentials/public_data.h"

static int client_credential_fetch(void* user_context,
                                   struct edhoc_auth_creds* credentials) {
  return edh_cred_auth_fetch(
      user_context, credentials, EDH_CRED_PUB_CLI_PK,
      ARRAY_SIZE(EDH_CRED_PUB_CLI_PK), EDH_CRED_CLI_PRIVATE_KEY,
      ARRAY_SIZE(EDH_CRED_CLI_PRIVATE_KEY), EDH_CRED_PUB_CLI_KID);
}

// ReSharper disable once CppParameterMayBeConstPtrOrRef (libedhoc signature
// forces it)
static int client_credential_verify(void* user_context,
                                    struct edhoc_auth_creds* credentials,
                                    const uint8_t** public_key_reference,
                                    size_t* public_key_length) {
  return edh_cred_auth_verify(
      user_context, credentials, EDH_CRED_PUB_SRV_KID, EDH_CRED_PUB_SRV_PK,
      ARRAY_SIZE(EDH_CRED_PUB_SRV_PK), public_key_reference, public_key_length);
}

edh_cli_handshake_status_t edh_cli_handshake_init(
    edh_cli_handshake_t* handshake) {
  if (!handshake) {
    return EDH_CLI_HANDSHAKE_INVALID_ARGUMENT;
  }

  memset(handshake, 0, sizeof(*handshake));

  const struct edhoc_credentials credentials = {
      .fetch = client_credential_fetch,
      .verify = client_credential_verify,
  };

  if (edh_com_setup_context(&handshake->context, &credentials) !=
      EDHOC_SUCCESS) {
    return EDH_CLI_HANDSHAKE_CONTEXT_SETUP_FAILED;
  }

  handshake->initialized = true;

  return EDH_CLI_HANDSHAKE_SUCCESS;
}

bool edh_cli_handshake_is_initialized(const edh_cli_handshake_t* handshake) {
  // ReSharper disable once CppDFANullDereference
  return handshake != NULL && handshake->initialized;
}

static bool edh_cli_composition_output_is_valid(const uint8_t* payload, const size_t* payload_len) {
  return payload != NULL && payload_len != NULL;
}

static bool edh_cli_message_payload_is_valid(const uint8_t* payload,
                                             const size_t payload_len) {
  return payload != NULL && payload_len > 0;
}

edh_cli_handshake_status_t edh_cli_handshake_compose_message_1(
    edh_cli_handshake_t* handshake, const size_t payload_capacity,
    uint8_t* payload, size_t* payload_len) {
  const size_t min_prefix_payload_capacity = 2;
  if (!edh_cli_handshake_is_initialized(handshake) ||
      !edh_cli_composition_output_is_valid(payload, payload_len) ||
      payload_capacity < min_prefix_payload_capacity) {
    return EDH_CLI_HANDSHAKE_INVALID_ARGUMENT;
  }

  payload[0] = EDH_COM_CBOR_TRUE;
  size_t message_len = 0;
  if (edhoc_message_1_compose(&handshake->context, &payload[1],
                              payload_capacity - 1,
                              &message_len) != EDHOC_SUCCESS) {
    return EDH_CLI_HANDSHAKE_MESSAGE_1_COMPOSE_FAILED;
  }

  *payload_len = message_len + 1;
  return EDH_CLI_HANDSHAKE_SUCCESS;
}

edh_cli_handshake_status_t edh_cli_handshake_process_message_2(
    edh_cli_handshake_t* handshake, const uint8_t* payload,
    const size_t payload_len) {
  if (!edh_cli_handshake_is_initialized(handshake) ||
      !edh_cli_message_payload_is_valid(payload, payload_len)) {
    return EDH_CLI_HANDSHAKE_INVALID_ARGUMENT;
  }

  if (edhoc_message_2_process(&handshake->context, payload, payload_len) !=
      EDHOC_SUCCESS) {
    return EDH_CLI_HANDSHAKE_MESSAGE_2_PROCESS_FAILED;
  }

  return EDH_CLI_HANDSHAKE_SUCCESS;
}

edh_cli_handshake_status_t edh_cli_handshake_compose_message_3(
    edh_cli_handshake_t* handshake, const size_t payload_capacity,
    uint8_t* payload, size_t* payload_len) {
  const size_t min_payload_capacity = 1;
  if (!edh_cli_handshake_is_initialized(handshake) ||
      !edh_cli_composition_output_is_valid(payload, payload_len) ||
      payload_capacity < min_payload_capacity) {
    return EDH_CLI_HANDSHAKE_INVALID_ARGUMENT;
  }

  struct edhoc_prepended_fields prepended_fields = {
      .buffer = payload,
      .buffer_size = payload_capacity,
      .edhoc_message_ptr = payload,
      .edhoc_message_size = payload_capacity,
  };

  if (edhoc_prepend_connection_id(&prepended_fields,
                                  &handshake->context.private_peer_cid) !=
      EDHOC_SUCCESS) {
    return EDH_CLI_HANDSHAKE_CONNECTION_ID_PREPEND_FAILED;
  }

  size_t message3_len = 0;
  if (edhoc_message_3_compose(&handshake->context,
                              prepended_fields.edhoc_message_ptr,
                              prepended_fields.edhoc_message_size,
                              &message3_len) != EDHOC_SUCCESS) {
    return EDH_CLI_HANDSHAKE_MESSAGE_3_COMPOSE_FAILED;
  }

  prepended_fields.edhoc_message_size = message3_len;
  if (edhoc_prepend_recalculate_size(&prepended_fields) != EDHOC_SUCCESS) {
    return EDH_CLI_HANDSHAKE_PREPEND_RECALCULATION_FAILED;
  }

  *payload_len = prepended_fields.buffer_size;
  return EDH_CLI_HANDSHAKE_SUCCESS;
}

edh_cli_handshake_status_t edh_cli_handshake_process_message_4(
    edh_cli_handshake_t* handshake, const uint8_t* payload,
    const size_t payload_len) {
  if (!edh_cli_handshake_is_initialized(handshake) ||
      !edh_cli_message_payload_is_valid(payload, payload_len)) {
    return EDH_CLI_HANDSHAKE_INVALID_ARGUMENT;
  }

  if (edhoc_message_4_process(&handshake->context, payload, payload_len) !=
      EDHOC_SUCCESS) {
    return EDH_CLI_HANDSHAKE_MESSAGE_4_PROCESS_FAILED;
  }

  return EDH_CLI_HANDSHAKE_SUCCESS;
}

void edh_cli_handshake_deinit(edh_cli_handshake_t* handshake) {
  if (!edh_cli_handshake_is_initialized(handshake)) {
    return;
  }

  (void)edhoc_context_deinit(&handshake->context);
  memset(handshake, 0, sizeof(*handshake));
}
