#include "server.h"

#include <coap3/coap.h>
#include <coap3/coap_session.h>

#include "coap/server_edhoc_dispatch.h"
#include "coap/server_utils.h"
#include "common/cleanup.h"

static void edhoc_post_handler(coap_resource_t* resource,
                               coap_session_t* session,
                               const coap_pdu_t* request,
                               const coap_string_t* query,
                               coap_pdu_t* response) {
  (void)resource;
  (void)query;
  coap_server_dispatch_edhoc_post(session, request, response);
}

emulation_status_t run_server(void) {
  session_resources_t server_resources = {0};

  server_resources.coap_context = coap_server_create_context();
  if (!server_resources.coap_context) {
    cleanup_resources(&server_resources);
    return EMULATION_FAILURE;
  }

  static const char COAP_LISTEN_UCAST_IP[] = "::";
  coap_status_result_t result = coap_server_setup_endpoints(
      server_resources.coap_context, COAP_LISTEN_UCAST_IP);
  if (result != COAP_STATUS_SUCCESS) {
    cleanup_resources(&server_resources);
    return EMULATION_FAILURE;
  }

  result = coap_server_add_post_resource(
      server_resources.coap_context, ".well-known/edhoc", edhoc_post_handler);
  if (result != COAP_STATUS_SUCCESS) {
    cleanup_resources(&server_resources);
    return EMULATION_FAILURE;
  }

  // coap_session_set_app_data2(server_resources.coap_session,
  //                            &edhoc_session_context, NULL);

  result = coap_server_run_input_output_loop(server_resources.coap_context);
  if (result != COAP_STATUS_SUCCESS) {
    cleanup_resources(&server_resources);
    return EMULATION_FAILURE;
  }

  cleanup_resources(&server_resources);
  return EMULATION_SUCCESS;
}

// static int run_handshake(struct edhoc_context* context, int* socket_fd,
//                          struct sockaddr_in* server_address,
//                          struct sockaddr_in* client_address) {
//   socklen_t client_address_len = sizeof(*client_address);

//   printf("Server listening on port %d...\n",
//   ntohs(server_address->sin_port));

//   uint8_t message_buffer[MESSAGE_BUFFER_LENGTH] = {0};
//   const size_t message_buffer_size = sizeof(message_buffer);

//   ssize_t message1_length =
//       recvfrom(*socket_fd, message_buffer, message_buffer_size, 0,
//                (struct sockaddr*)client_address, &client_address_len);
//   printf("Received Message 1 (%zd bytes)\n", message1_length);

//   if (edhoc_message_1_process(context, message_buffer, message1_length) !=
//       EDHOC_SUCCESS) {
//     fprintf(stderr, "cannot process Message 1\n");
//     return -1;
//   }

//   size_t message2_length = 0;
//   if (edhoc_message_2_compose(context, message_buffer, message_buffer_size,
//                               &message2_length) != EDHOC_SUCCESS) {
//     fprintf(stderr, "cannot compose Message 2\n");
//     return -1;
//   }

//   sendto(*socket_fd, message_buffer, message2_length, 0,
//          (const struct sockaddr*)client_address, client_address_len);
//   printf("Sent Message 2 (%zu bytes)\n", message2_length);

//   ssize_t message3_length =
//       recvfrom(*socket_fd, message_buffer, message_buffer_size, 0,
//                (struct sockaddr*)client_address, &client_address_len);
//   printf("Received Message 3 (%zd bytes)\n", message3_length);

//   if (edhoc_message_3_process(context, message_buffer, message3_length) !=
//       EDHOC_SUCCESS) {
//     fprintf(stderr, "cannot process Message 3\n");
//     return -1;
//   }

//   printf("Server: EDHOC Handshake Completed Successfully!\n");

//   return EDHOC_SUCCESS;
// }

// /**
//  * @brief Receive and decrypt a message from the client after handshake
//  * @param context EDHOC context for exporting shared secret
//  * @param socket_fd UDP socket file descriptor
//  * @param client_address Client address structure
//  * @param received_message_buffer_size Size of output buffer
//  * @param[out] received_message_buffer Output buffer for decrypted message
//  * @param[out] received_message_length Actual length of decrypted message
//  * @return Status code
//  */
// static int receive_message(struct edhoc_context* context, int socket_fd,
//                            struct sockaddr_in* client_address,
//                            size_t received_message_buffer_size,
//                            uint8_t* received_message_buffer,
//                            size_t* received_message_length) {
//   uint8_t shared_secret[EXPORTED_SECRET_LENGTH] = {0};
//   int result = edhoc_export_prk_exporter(context, PKR_OUT_LABEL,
//   shared_secret,
//                                          sizeof(shared_secret));
//   if (result != EDHOC_SUCCESS) {
//     return result;
//   }

//   uint8_t encrypted_message[CIPHERTEXT_MAX_LENGTH] = {0};
//   socklen_t client_address_len = sizeof(*client_address);
//   result = recvfrom(socket_fd, encrypted_message,
//   sizeof(encrypted_message), 0,
//                     (struct sockaddr*)client_address, &client_address_len);
//   if (result < 0) {
//     fprintf(stderr, "cannot receive ciphertext\n");
//     return -1;
//   }
//   if (result == 0) {
//     fprintf(stderr, "Received empty message\n");
//     return -1;
//   }

//   size_t encrypted_message_length = (size_t)result;
//   const psa_status_t decrypt_status = shared_decrypt_ciphertext(
//       encrypted_message, encrypted_message_length, shared_secret,
//       sizeof(shared_secret), received_message_buffer,
//       received_message_buffer_size, received_message_length);
//   if (decrypt_status != PSA_SUCCESS) {
//     fprintf(stderr, "cannot decrypt ciphertext\n");
//   }
//   return EDHOC_SUCCESS;
// }