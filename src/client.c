#include "client.h"

#include <coap3/coap.h>
#include <stdio.h>

#include "coap/client_edhoc_exchange.h"
#include "coap/client_utils.h"
#include "coap/common/config.h"
#include "common/cleanup.h"
#include "edhoc/client/client_flow.h"
#include "edhoc/common/config.h"

// TODO: SEND ERROR RESPONSES TO SERVER IN CASE OF FAILURE INSTEAD OF JUST
// FAILING SILENTLY

emulation_status_t run_client(void) {
  coap_startup();
  coap_set_log_level(COAP_LOG_WARN);

  session_resources_t client_resources = {0};
  client_edhoc_flow_t* flow = NULL;
  client_edhoc_exchange_t* exchange = NULL;

  static const char CLIENT_COAP_URI[] =
      "coap://localhost:5683/.well-known/edhoc";
  coap_uri_t client_uri = {0};
  coap_address_t destination_address = {0};
  if (parse_and_resolve_coap_uri(CLIENT_COAP_URI, &client_uri,
                                 &destination_address) != COAP_STATUS_SUCCESS) {
    cleanup_resources(&client_resources);
    return EMULATION_FAILURE;
  }
  if (create_coap_client_session(&client_uri, &destination_address, NULL,
                                 &client_resources.coap_context,
                                 &client_resources.coap_session) !=
      COAP_STATUS_SUCCESS) {
    cleanup_resources(&client_resources);
    return EMULATION_FAILURE;
  }

  if (client_edhoc_exchange_init(client_resources.coap_context,
                                 client_resources.coap_session, &client_uri,
                                 &destination_address,
                                 &exchange) != COAP_STATUS_SUCCESS) {
    cleanup_resources(&client_resources);
    return EMULATION_FAILURE;
  }

  if (client_edhoc_flow_init(&flow) != CLIENT_EDHOC_FLOW_SUCCESS) {
    client_edhoc_exchange_deinit(&exchange);
    cleanup_resources(&client_resources);
    return EMULATION_FAILURE;
  }

  uint8_t request_payload[MESSAGE_BUFFER_LENGTH] = {0};
  size_t request_len = 0;
  uint8_t response_payload[MAX_PDU_SIZE] = {0};
  size_t response_len = 0;

  if (client_edhoc_flow_compose_message_1(flow, MESSAGE_BUFFER_LENGTH,
                                          request_payload, &request_len) !=
      CLIENT_EDHOC_FLOW_SUCCESS) {
    client_edhoc_flow_deinit(&flow);
    client_edhoc_exchange_deinit(&exchange);
    cleanup_resources(&client_resources);
    return EMULATION_FAILURE;
  }

  if (client_edhoc_exchange_send(exchange, request_payload, request_len,
                                 APPLICATION_CID_EDHOC_CBOR_SEQ) !=
      COAP_STATUS_SUCCESS) {
    client_edhoc_flow_deinit(&flow);
    client_edhoc_exchange_deinit(&exchange);
    cleanup_resources(&client_resources);
    return EMULATION_FAILURE;
  }

  if (client_edhoc_exchange_wait_and_get(exchange, MAX_PDU_SIZE,
                                         response_payload, &response_len) !=
          COAP_STATUS_SUCCESS ||
      client_edhoc_flow_process_message_2(
          flow, response_payload, response_len) != CLIENT_EDHOC_FLOW_SUCCESS) {
    client_edhoc_flow_deinit(&flow);
    client_edhoc_exchange_deinit(&exchange);
    cleanup_resources(&client_resources);
    return EMULATION_FAILURE;
  }

  client_edhoc_exchange_reset(exchange);

  if (client_edhoc_flow_compose_message_3(flow, MESSAGE_BUFFER_LENGTH,
                                          request_payload, &request_len) !=
          CLIENT_EDHOC_FLOW_SUCCESS ||
      client_edhoc_exchange_send(exchange, request_payload, request_len,
                                 APPLICATION_CID_EDHOC_CBOR_SEQ) !=
          COAP_STATUS_SUCCESS ||
      client_edhoc_exchange_wait_and_get(exchange, MAX_PDU_SIZE,
                                         response_payload, &response_len) !=
          COAP_STATUS_SUCCESS ||
      client_edhoc_flow_process_message_4(
          flow, response_payload, response_len) != CLIENT_EDHOC_FLOW_SUCCESS) {
    client_edhoc_flow_deinit(&flow);
    client_edhoc_exchange_deinit(&exchange);
    cleanup_resources(&client_resources);
    return EMULATION_FAILURE;
  }

  printf("Client: EDHOC Handshake Completed Successfully!\n");

  client_edhoc_flow_deinit(&flow);
  client_edhoc_exchange_deinit(&exchange);
  cleanup_resources(&client_resources);
  return EMULATION_SUCCESS;
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
