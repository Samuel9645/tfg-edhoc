#include "coap3/coap.h"
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
    fprintf(stderr, "Server: Failed to export PRK exporter\n");
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
  coap_startup();
  coap_set_log_level(COAP_LOG_WARN);
  coap_context_t* coap_context = coap_new_context(NULL);
  if (!coap_context) {
    coap_log_emerg("cannot create libcoap context\n");
    return end_coap_session(NULL, NULL, coap_context);
  }

  coap_context_set_block_mode(coap_context,
                              USE_LIBCOAP_FOR_REQUEST_AND_SINGLE_BODY_DATA);
  const bool HAS_PKI_PSK_INFO = false;
  const bool USE_WEB_SOCKETS = false;
  uint32_t scheme_hints_bits = coap_get_available_scheme_hint_bits(
      HAS_PKI_PSK_INFO, USE_WEB_SOCKETS, COAP_PROTO_NONE);

  static const char COAP_LISTEN_UCAST_IP[] = "::";
  const coap_str_const_t* listen_address =
      coap_make_str_const(COAP_LISTEN_UCAST_IP);
  enum { USE_DEFAULT_PORT_DATA = 0, NO_AI_HINT_FLAGS = 0 };
  coap_addr_info_t* endpoint_info_list = coap_resolve_address_info(
      listen_address, USE_DEFAULT_PORT_DATA, USE_DEFAULT_PORT_DATA,
      USE_DEFAULT_PORT_DATA, USE_DEFAULT_PORT_DATA, NO_AI_HINT_FLAGS,
      scheme_hints_bits, COAP_RESOLVE_TYPE_LOCAL);

  bool has_endpoint = false;
  for (coap_addr_info_t* endpoint_info = endpoint_info_list;
       endpoint_info != NULL; endpoint_info = endpoint_info->next) {
    coap_endpoint_t* endpoint = coap_new_endpoint(
        coap_context, &endpoint_info->addr, endpoint_info->proto);
    if (!endpoint) {
      coap_log_warn("cannot create endpoint for CoAP proto %u\n",
                    endpoint_info->proto);
    } else {
      has_endpoint = true;
    }
  }
  coap_free_address_info(endpoint_info_list);
  if (!has_endpoint) {
    coap_log_err("No context available for interface '%s'\n",
                 (const char*)listen_address->s);
    return end_coap_session(NULL, NULL, coap_context);
  }

  static const char COAP_LISTEN_MCAST_IPV6[] = "ff02::fd";
  coap_join_mcast_group_intf(coap_context, COAP_LISTEN_MCAST_IPV6, NULL);

  /* Create a resource that the server can respond to with information */
  enum { MEMORY_HANDLING_FLAGS = 0 };
  coap_resource_t* resource =
      coap_resource_init(coap_make_str_const("hello"), MEMORY_HANDLING_FLAGS);
  if (!resource) {
    coap_log_emerg("cannot create resource\n");
    return end_coap_session(NULL, NULL, coap_context);
  }
  coap_register_request_handler(resource, COAP_REQUEST_GET,
                                first_resource_get_handler);
  coap_add_resource(coap_context, resource);

  resource = coap_resource_init(coap_make_str_const("hello/my"),
                                MEMORY_HANDLING_FLAGS);
  if (!resource) {
    coap_log_emerg("cannot create resource\n");
    return end_coap_session(NULL, NULL, coap_context);
  }
  coap_register_request_handler(resource, COAP_REQUEST_GET,
                                second_resource_get_handler);
  while (true) {
    coap_io_process(coap_context, COAP_IO_WAIT);
  }
  end_coap_session(NULL, NULL, coap_context);
  return 0;
}
