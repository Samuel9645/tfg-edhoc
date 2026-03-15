#include "coap3/coap.h"
#include "coap_server_utils.h"
#include "coap_shared.h"
#include "server.h"
#include "shared_credentials.h"
#include "shared_crypto.h"

static int credential_fetch(void* user_context,
                            struct edhoc_auth_creds* credentials) {
  return shared_credential_fetch(user_context, credentials, SERVER_PUBLIC_KEY,
                                 sizeof(SERVER_PUBLIC_KEY), SERVER_PRIVATE_KEY,
                                 sizeof(SERVER_PRIVATE_KEY), SERVER_KID);
}

static int credential_verify(void* user_context,
                             struct edhoc_auth_creds* credentials,
                             const uint8_t** public_key_reference,
                             size_t* public_key_length) {
  int result = shared_credential_verify(
      user_context, credentials, CLIENT_KID, CLIENT_PUBLIC_KEY,
      ARRAY_SIZE(CLIENT_PUBLIC_KEY), public_key_reference, public_key_length);

  if (result == EDHOC_SUCCESS) {
    initialize_credential_key(credentials, CLIENT_PUBLIC_KEY,
                              ARRAY_SIZE(CLIENT_PUBLIC_KEY), CLIENT_KID);
  }

  return result;
}

static int run_handshake(struct edhoc_context* context, int* socket_fd,
                         struct sockaddr_in* server_address,
                         struct sockaddr_in* client_address) {
  socklen_t client_address_len = sizeof(*client_address);

  printf("Server listening on port %d...\n", ntohs(server_address->sin_port));

  uint8_t message_buffer[MESSAGE_BUFFER_LENGTH] = {0};
  const size_t message_buffer_size = sizeof(message_buffer);

  ssize_t message1_length =
      recvfrom(*socket_fd, message_buffer, message_buffer_size, 0,
               (struct sockaddr*)client_address, &client_address_len);
  printf("Received Message 1 (%zd bytes)\n", message1_length);

  if (edhoc_message_1_process(context, message_buffer, message1_length) !=
      EDHOC_SUCCESS) {
    fprintf(stderr, "Failed to process Message 1\n");
    return -1;
  }

  size_t message2_length = 0;
  if (edhoc_message_2_compose(context, message_buffer, message_buffer_size,
                              &message2_length) != EDHOC_SUCCESS) {
    fprintf(stderr, "Failed to compose Message 2\n");
    return -1;
  }

  sendto(*socket_fd, message_buffer, message2_length, 0,
         (const struct sockaddr*)client_address, client_address_len);
  printf("Sent Message 2 (%zu bytes)\n", message2_length);

  ssize_t message3_length =
      recvfrom(*socket_fd, message_buffer, message_buffer_size, 0,
               (struct sockaddr*)client_address, &client_address_len);
  printf("Received Message 3 (%zd bytes)\n", message3_length);

  if (edhoc_message_3_process(context, message_buffer, message3_length) !=
      EDHOC_SUCCESS) {
    fprintf(stderr, "Failed to process Message 3\n");
    return -1;
  }

  printf("Server: EDHOC Handshake Completed Successfully!\n");

  return EDHOC_SUCCESS;
}

/**
 * @brief Receive and decrypt a message from the client after handshake
 * @param context EDHOC context for exporting shared secret
 * @param socket_fd UDP socket file descriptor
 * @param client_address Client address structure
 * @param received_message_buffer_size Size of output buffer
 * @param[out] received_message_buffer Output buffer for decrypted message
 * @param[out] received_message_length Actual length of decrypted message
 * @return Status code
 */
static int receive_message(struct edhoc_context* context, int socket_fd,
                           struct sockaddr_in* client_address,
                           size_t received_message_buffer_size,
                           uint8_t* received_message_buffer,
                           size_t* received_message_length) {
  uint8_t shared_secret[EXPORTED_SECRET_LENGTH] = {0};
  int result = edhoc_export_prk_exporter(context, PKR_OUT_LABEL, shared_secret,
                                         sizeof(shared_secret));
  if (result != EDHOC_SUCCESS) {
    return result;
  }

  uint8_t encrypted_message[CIPHERTEXT_MAX_LENGTH] = {0};
  socklen_t client_address_len = sizeof(*client_address);
  result = recvfrom(socket_fd, encrypted_message, sizeof(encrypted_message), 0,
                    (struct sockaddr*)client_address, &client_address_len);
  if (result < 0) {
    fprintf(stderr, "Failed to receive ciphertext\n");
    return -1;
  }
  if (result == 0) {
    fprintf(stderr, "Received empty message\n");
    return -1;
  }

  size_t encrypted_message_length = (size_t)result;
  const psa_status_t decrypt_status = shared_decrypt_ciphertext(
      encrypted_message, encrypted_message_length, shared_secret,
      sizeof(shared_secret), received_message_buffer,
      received_message_buffer_size, received_message_length);
  if (decrypt_status != PSA_SUCCESS) {
    fprintf(stderr, "Failed to decrypt ciphertext\n");
  }
  return EDHOC_SUCCESS;
}

static void first_resource_get_handler(coap_resource_t* resource,
                                       coap_session_t* session,
                                       const coap_pdu_t* request,
                                       const coap_string_t* query,
                                       coap_pdu_t* response) {
  (void)resource;
  (void)session;
  (void)query;

  coap_show_pdu(COAP_LOG_WARN, request);
  coap_pdu_set_code(response, COAP_RESPONSE_CODE_CONTENT);
  coap_add_data(response, 5, (const uint8_t*)"world");
  coap_show_pdu(COAP_LOG_WARN, response);
}

static void second_resource_get_handler(coap_resource_t* resource,
                                        coap_session_t* session,
                                        const coap_pdu_t* request,
                                        const coap_string_t* query,
                                        coap_pdu_t* response) {
  (void)resource;
  (void)session;
  (void)query;

  coap_show_pdu(COAP_LOG_WARN, request);
  coap_pdu_set_code(response, COAP_RESPONSE_CODE_CONTENT);
  coap_add_data(response, 8, (const uint8_t*)"my world");
  coap_show_pdu(COAP_LOG_WARN, response);
}

/**
 * @brief Execute EDHOC server handshake and receive encrypted message from
 * client
 *
 * This function binds to a UDP socket, performs the complete EDHOC
 * handshake as a responder, and receives an encrypted message from the
 * client.
 *
 * @return 0 on success, negative error code on failure
 */
int run_server() {
  // struct edhoc_context context = {0};
  // const struct edhoc_credentials credentials = {
  //     .fetch = credential_fetch,
  //     .verify = credential_verify,
  // };

  // int result = edhoc_setup_context(&context, &credentials);
  // if (result != EDHOC_SUCCESS) {
  //   edhoc_context_deinit(&context);
  //   return result;
  // }

  // int socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
  // if (socket_fd < 0) {
  //   fprintf(stderr, "Socket creation failed\n");
  //   return -1;
  // }

  // struct sockaddr_in server_address;
  // struct sockaddr_in client_address;
  // memset(&server_address, 0, sizeof(server_address));
  // memset(&client_address, 0, sizeof(client_address));

  // server_address.sin_addr.s_addr = htonl(INADDR_ANY);
  // server_address.sin_port = htons(SERVER_PORT);
  // server_address.sin_family = AF_INET;

  // if (bind(socket_fd, (const struct sockaddr*)&server_address,
  //          sizeof(server_address)) < 0) {
  //   fprintf(stderr, "Bind failed\n");
  //   close(socket_fd);
  //   return -1;
  // }

  // result =
  //     run_handshake(&context, &socket_fd, &server_address, &client_address);
  // if (result != EDHOC_SUCCESS) {
  //   fprintf(stderr, "Failed to run handshake\n");
  //   close(socket_fd);
  //   edhoc_context_deinit(&context);
  //   return result;
  // }

  // uint8_t received_message[MESSAGE_BUFFER_LENGTH] = {0};
  // size_t received_message_length = 0;
  // result = receive_message(&context, socket_fd, &client_address,
  //                          sizeof(received_message), received_message,
  //                          &received_message_length);
  // if (result != EDHOC_SUCCESS) {
  //   close(socket_fd);
  //   edhoc_context_deinit(&context);
  //   return result;
  // }
  // printf("Server: Received message from client: %.*s\n",
  //        (int)received_message_length, received_message);

  // close(socket_fd);
  // edhoc_context_deinit(&context);
  coap_context_t* coap_context = coap_server_create_context();
  if (!coap_context) {
    return end_coap_session(NULL, NULL, coap_context);
  }

  static const char COAP_LISTEN_UCAST_IP[] = "::";
  CoapServerUtilsResult result =
      coap_server_setup_endpoints(coap_context, COAP_LISTEN_UCAST_IP);
  if (result != COAP_SERVER_UTILS_SUCCESS) {
    return end_coap_session(NULL, NULL, coap_context);
  }

  static const char COAP_LISTEN_MCAST_IPV6[] = "ff02::fd";
  result =
      coap_server_join_multicast_group(coap_context, COAP_LISTEN_MCAST_IPV6);
  if (result != COAP_SERVER_UTILS_SUCCESS) {
    return end_coap_session(NULL, NULL, coap_context);
  }

  result = coap_server_add_post_resource(coap_context, ".well-known/edhoc",
                                         first_resource_get_handler);
  if (result != COAP_SERVER_UTILS_SUCCESS) {
    return end_coap_session(NULL, NULL, coap_context);
  }

  result = coap_server_add_post_resource(coap_context, "hello/my",
                                         second_resource_get_handler);
  if (result != COAP_SERVER_UTILS_SUCCESS) {
    return end_coap_session(NULL, NULL, coap_context);
  }

  result = coap_server_run_input_output_loop(coap_context);
  if (result != COAP_SERVER_UTILS_SUCCESS) {
    return end_coap_session(NULL, NULL, coap_context);
  }

  end_coap_session(NULL, NULL, coap_context);
  return 0;
}
