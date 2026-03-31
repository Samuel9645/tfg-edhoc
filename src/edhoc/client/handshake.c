#include "edhoc/client/handshake.h"

#include <edhoc_helpers.h>
#include <edhoc_values.h>
#include <string.h>

#include "coap/coap_config.h"
#include "edhoc/common/setup.h"
#include "edhoc/config.h"
#include "edhoc/credentials/authentication.h"
#include "edhoc/credentials/client_private_key.h"
#include "edhoc/credentials/public_data.h"

static int client_credential_fetch(void* user_context,
                                   struct edhoc_auth_creds* credentials) {
  return edhoc_credentials_fetch(user_context, credentials, CLIENT_PUBLIC_KEY,
                                 ARRAY_SIZE(CLIENT_PUBLIC_KEY),
                                 CLIENT_PRIVATE_KEY,
                                 ARRAY_SIZE(CLIENT_PRIVATE_KEY), CLIENT_KID);
}

static int client_credential_verify(void* user_context,
                                    struct edhoc_auth_creds* credentials,
                                    const uint8_t** public_key_reference,
                                    size_t* public_key_length) {
  return edhoc_credentials_verify(
      user_context, credentials, SERVER_KID, SERVER_PUBLIC_KEY,
      ARRAY_SIZE(SERVER_PUBLIC_KEY), public_key_reference, public_key_length);
}

edhoc_client_handshake_status_t edhoc_client_handshake_init(
    edhoc_client_handshake_t* handshake) {
  if (!handshake) {
    return EDHOC_CLIENT_HANDSHAKE_INVALID_ARGUMENT;
  }

  memset(handshake, 0, sizeof(*handshake));

  const struct edhoc_credentials credentials = {
      .fetch = client_credential_fetch,
      .verify = client_credential_verify,
  };

  if (edhoc_common_setup_context(&handshake->context, &credentials) !=
      EDHOC_SUCCESS) {
    return EDHOC_CLIENT_HANDSHAKE_CONTEXT_SETUP_FAILED;
  }

  handshake->initialized = true;

  return EDHOC_CLIENT_HANDSHAKE_SUCCESS;
}

/**
 * @brief Validate output buffer parameters for message composition.
 *
 * Checks that payload buffer, size output pointer, and capacity are all valid.
 *
 * @param[in] payload Output payload buffer.
 * @param[in] payload_len Output size pointer.
 * @param[in] min_capacity Minimum required buffer capacity.
 * @return true if parameters are valid and capacity >= min_capacity.
 */
static inline bool edhoc_client_composition_output_is_valid(
    const uint8_t* payload, const size_t* payload_len, size_t min_capacity) {
  return (payload != NULL) && (payload_len != NULL) && (min_capacity > 0);
}

/**
 * @brief Validate input payload parameters for message processing.
 *
 * @param[in] payload Input payload buffer.
 * @param[in] payload_len Payload length.
 * @return true if parameters are valid, false otherwise.
 */
static inline bool edhoc_client_message_payload_is_valid(const uint8_t* payload,
                                                         size_t payload_len) {
  return (payload != NULL) && (payload_len > 0);
}

edhoc_client_handshake_status_t edhoc_client_handshake_compose_message_1(
    edhoc_client_handshake_t* handshake, size_t payload_capacity,
    uint8_t* payload, size_t* payload_len) {
  const size_t min_prefix_payload_capacity = 2;
  if (!edhoc_client_handshake_is_initialized(handshake) ||
      !edhoc_client_composition_output_is_valid(payload, payload_len,
                                                min_prefix_payload_capacity)) {
    return EDHOC_CLIENT_HANDSHAKE_INVALID_ARGUMENT;
  }
  if (payload_capacity <= 1) {
    return EDHOC_CLIENT_HANDSHAKE_INVALID_ARGUMENT;
  }

  payload[0] = EDC_CBOR_TRUE;
  size_t message_len = 0;
  if (edhoc_message_1_compose(&handshake->context, &payload[1],
                              payload_capacity - 1,
                              &message_len) != EDHOC_SUCCESS) {
    return EDHOC_CLIENT_HANDSHAKE_MESSAGE_1_COMPOSE_FAILED;
  }

  *payload_len = message_len + 1;
  return EDHOC_CLIENT_HANDSHAKE_SUCCESS;
}

edhoc_client_handshake_status_t edhoc_client_handshake_process_message_2(
    edhoc_client_handshake_t* handshake, const uint8_t* payload,
    size_t payload_len) {
  if (!edhoc_client_handshake_is_initialized(handshake) ||
      !edhoc_client_message_payload_is_valid(payload, payload_len)) {
    return EDHOC_CLIENT_HANDSHAKE_INVALID_ARGUMENT;
  }

  if (edhoc_message_2_process(&handshake->context, payload, payload_len) !=
      EDHOC_SUCCESS) {
    return EDHOC_CLIENT_HANDSHAKE_MESSAGE_2_PROCESS_FAILED;
  }

  return EDHOC_CLIENT_HANDSHAKE_SUCCESS;
}

edhoc_client_handshake_status_t edhoc_client_handshake_compose_message_3(
    edhoc_client_handshake_t* handshake, size_t payload_capacity,
    uint8_t* payload, size_t* payload_len) {
  const size_t min_payload_capacity = 1;
  if (!edhoc_client_handshake_is_initialized(handshake) ||
      !edhoc_client_composition_output_is_valid(payload, payload_len,
                                                min_payload_capacity)) {
    return EDHOC_CLIENT_HANDSHAKE_INVALID_ARGUMENT;
  }
  if (payload_capacity == 0) {
    return EDHOC_CLIENT_HANDSHAKE_INVALID_ARGUMENT;
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
    return EDHOC_CLIENT_HANDSHAKE_CONNECTION_ID_PREPEND_FAILED;
  }

  size_t message3_len = 0;
  if (edhoc_message_3_compose(&handshake->context,
                              prepended_fields.edhoc_message_ptr,
                              prepended_fields.edhoc_message_size,
                              &message3_len) != EDHOC_SUCCESS) {
    return EDHOC_CLIENT_HANDSHAKE_MESSAGE_3_COMPOSE_FAILED;
  }

  prepended_fields.edhoc_message_size = message3_len;
  if (edhoc_prepend_recalculate_size(&prepended_fields) != EDHOC_SUCCESS) {
    return EDHOC_CLIENT_HANDSHAKE_PREPEND_RECALCULATION_FAILED;
  }

  *payload_len = prepended_fields.buffer_size;
  return EDHOC_CLIENT_HANDSHAKE_SUCCESS;
}

edhoc_client_handshake_status_t edhoc_client_handshake_process_message_4(
    edhoc_client_handshake_t* handshake, const uint8_t* payload,
    size_t payload_len) {
  if (!edhoc_client_handshake_is_initialized(handshake) ||
      !edhoc_client_message_payload_is_valid(payload, payload_len)) {
    return EDHOC_CLIENT_HANDSHAKE_INVALID_ARGUMENT;
  }

  if (edhoc_message_4_process(&handshake->context, payload, payload_len) !=
      EDHOC_SUCCESS) {
    return EDHOC_CLIENT_HANDSHAKE_MESSAGE_4_PROCESS_FAILED;
  }

  return EDHOC_CLIENT_HANDSHAKE_SUCCESS;
}

void edhoc_client_handshake_deinit(edhoc_client_handshake_t* handshake) {
  if (!edhoc_client_handshake_is_initialized(handshake)) {
    return;
  }

  (void)edhoc_context_deinit(&handshake->context);
  memset(handshake, 0, sizeof(*handshake));
}
