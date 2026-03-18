#include "client.h"

#include <coap3/coap.h>
#include <edhoc_values.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "coap/client_utils.h"
#include "coap/common/helpers.h"
#include "coap/common/status.h"
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

static bool have_response = false;
enum { MAX_PDU_SIZE = 1024 };
static uint8_t incoming_message_buffer[MAX_PDU_SIZE] = {0};
static size_t incoming_message_length = 0;

static coap_response_t response_handler(coap_session_t* session,
                                        const coap_pdu_t* sent,
                                        const coap_pdu_t* received,
                                        const coap_mid_t id) {
  (void)session;
  (void)sent;
  (void)id;

  have_response = true;
  coap_show_pdu(COAP_LOG_WARN, received);

  size_t len;
  const uint8_t* databuf;
  size_t offset;
  size_t total;
  enum { ERROR_CODE = 0 };
  if (coap_get_data_large(received, &len, &databuf, &offset, &total) ==
      ERROR_CODE) {
    coap_log_err("cannot get response data in handler\n");
    return COAP_RESPONSE_FAIL;
  }
  if (total > MAX_PDU_SIZE) {
    coap_log_err("response data too large\n");
    return COAP_RESPONSE_FAIL;
  }
  memcpy(incoming_message_buffer, databuf, len);
  incoming_message_length = len;
  // TODO: REMOVE THIS PRINT STATEMENT
  fprintf(stdout, "Received response data (%zu bytes)\n", len);

  return COAP_RESPONSE_OK;
}

/**
 * @brief Execute EDHOC client handshake and send encrypted message to server
 *
 * This function performs the complete EDHOC handshake as an initiator,
 * establishes a secure context, and sends an encrypted message to the server.
 *
 * @return EDHOC_SUCCESS on success, error code on failure
 */
int run_client(void) {
  coap_startup();
  coap_set_log_level(COAP_LOG_WARN);

  static const char CLIENT_COAP_URI[] =
      "coap://localhost:5683/.well-known/edhoc";
  coap_uri_t client_uri = {0};
  coap_address_t destination_address = {0};
  coap_status_result_t coap_result = parse_and_resolve_coap_uri(
      CLIENT_COAP_URI, &client_uri, &destination_address);
  if (coap_result != COAP_STATUS_SUCCESS) {
    return end_coap_session(NULL, NULL, NULL);
  }

  coap_context_t* coap_session_context = NULL;
  coap_session_t* coap_session = NULL;
  coap_result = create_coap_client_session(
      &client_uri, &destination_address, response_handler,
      &coap_session_context, &coap_session);
  if (coap_result != COAP_STATUS_SUCCESS) {
    return end_coap_session(NULL, coap_session, coap_session_context);
  }

  coap_optlist_t* optlist = create_coap_edhoc_optlist();
  if (!optlist) {
    return end_coap_session(NULL, coap_session, coap_session_context);
  }
  coap_pdu_t* protocol_data_unit = prepare_coap_post_request(
      &client_uri, &destination_address, coap_session, optlist);
  if (!protocol_data_unit) {
    return end_coap_session(optlist, coap_session, coap_session_context);
  }

  struct edhoc_context edhoc_session_context = {0};
  const struct edhoc_credentials credentials = {
      .fetch = client_credential_fetch,
      .verify = client_credential_verify,
  };

  int edhoc_result = edhoc_setup_context(&edhoc_session_context, &credentials);
  if (edhoc_result != EDHOC_SUCCESS) {
    edhoc_context_deinit(&edhoc_session_context);
    return end_coap_session(optlist, coap_session, coap_session_context);
  }

  // https://datatracker.ietf.org/doc/html/rfc9528/#name-the-forward-message-flow
  uint8_t payload_buffer[MESSAGE_BUFFER_LENGTH] = {0};
  enum { CBOR_TRUE = 0xF5 };
  payload_buffer[0] = CBOR_TRUE;
  size_t message1_length = 0;

  edhoc_result =
      edhoc_message_1_compose(&edhoc_session_context, &payload_buffer[1],
                              MESSAGE_BUFFER_LENGTH - 1, &message1_length);
  if (edhoc_result != EDHOC_SUCCESS) {
    edhoc_context_deinit(&edhoc_session_context);
    coap_log_err("cannot compose message 1\n");
    return end_coap_session(optlist, coap_session, coap_session_context);
  }

  const size_t TOTAL_PAYLOAD_LENGTH = message1_length + 1;
  enum { LIBCOAP_ERROR = 0 };
  if (coap_add_data(protocol_data_unit, TOTAL_PAYLOAD_LENGTH, payload_buffer) ==
      LIBCOAP_ERROR) {
    edhoc_context_deinit(&edhoc_session_context);
    coap_log_err("cannot add payload to PDU\n");
    return end_coap_session(optlist, coap_session, coap_session_context);
  }

  coap_show_pdu(COAP_LOG_WARN, protocol_data_unit);

  coap_result = send_coap_request(coap_session, protocol_data_unit);
  if (coap_result != COAP_STATUS_SUCCESS) {
    return end_coap_session(optlist, coap_session, coap_session_context);
  }

  coap_result = wait_for_coap_response(coap_session_context, coap_session,
                                       &have_response);
  if (coap_result != COAP_STATUS_SUCCESS) {
    return end_coap_session(optlist, coap_session, coap_session_context);
  }
  return 0;
}

// static int run_handshake(struct edhoc_context* context, int* socket_fd,
//                          struct sockaddr_in* server_address) {
//   printf("Client: Waiting for server to be ready...\n");
//   sleep(1);
//   uint8_t message_buffer[MESSAGE_BUFFER_LENGTH] = {0};
//   const size_t message_buffer_size = sizeof(message_buffer);

//   size_t message1_length = 0;
//   if (edhoc_message_1_compose(context, message_buffer, message_buffer_size,
//                               &message1_length) != EDHOC_SUCCESS) {
//     fprintf(stderr, "cannot compose Message 1\n");
//     close(*socket_fd);
//     return -1;
//   }

//   sendto(*socket_fd, message_buffer, message1_length, 0,
//          (const struct sockaddr*)server_address, sizeof(*server_address));
//   printf("Client: Sent Message 1 (%zu bytes)\n", message1_length);

//   socklen_t server_address_length = sizeof(*server_address);
//   ssize_t message2_length =
//       recvfrom(*socket_fd, message_buffer, message_buffer_size, 0,
//                (struct sockaddr*)server_address, &server_address_length);
//   if (message2_length < 0) {
//     fprintf(stderr,
//             "cannot receive Message 2 from server. Make sure server is "
//             "running!\n");
//     close(*socket_fd);
//     return -1;
//   }
//   printf("Client: Received Message 2 (%zd bytes)\n", message2_length);

//   if (edhoc_message_2_process(context, message_buffer, message2_length) !=
//       EDHOC_SUCCESS) {
//     fprintf(stderr, "Client: cannot process Message 2\n");
//     close(*socket_fd);
//     return -1;
//   }

//   size_t message3_length = 0;
//   if (edhoc_message_3_compose(context, message_buffer, message_buffer_size,
//                               &message3_length) != EDHOC_SUCCESS) {
//     fprintf(stderr, "Client: cannot compose Message 3\n");
//     close(*socket_fd);
//     return -1;
//   }

//   sendto(*socket_fd, message_buffer, message3_length, 0,
//          (const struct sockaddr*)server_address, sizeof(*server_address));
//   printf("Client: Sent Message 3 (%zu bytes)\n", message3_length);

//   printf("Client: EDHOC Handshake Completed Successfully!");
//   return EDHOC_SUCCESS;
// }

// static int send_message(struct edhoc_context* context, int socket_fd,
//                         const struct sockaddr_in* server_address,
//                         const uint8_t* plaintext_message) {
//   uint8_t shared_secret[EXPORTED_SECRET_LENGTH] = {0};
//   int result = edhoc_export_prk_exporter(context, PKR_OUT_LABEL,
//   shared_secret,
//                                          sizeof(shared_secret));
//   if (result != EDHOC_SUCCESS) {
//     fprintf(stderr, "Client: cannot export PRK exporter\n");
//     return result;
//   }

//   uint8_t encrypted_message[CIPHERTEXT_MAX_LENGTH] = {0};
//   size_t encrypted_message_length = 0;
//   const psa_status_t encrypt_status = shared_encrypt_plaintext(
//       plaintext_message, strlen((const char*)plaintext_message),
//       shared_secret, sizeof(shared_secret), encrypted_message,
//       sizeof(encrypted_message), &encrypted_message_length);
//   if (encrypt_status != PSA_SUCCESS) {
//     fprintf(stderr, "cannot encrypt the send message\n");
//     return encrypt_status;
//   }

//   sendto(socket_fd, encrypted_message, encrypted_message_length, 0,
//          (const struct sockaddr*)server_address, sizeof(*server_address));

//   return EDHOC_SUCCESS;
// }
