#include "client.h"
#include "coap3/coap.h"
#include "coap_shared.h"
#include "shared_credentials.h"
#include "shared_crypto.h"

static int credential_fetch(void* user_context,
                            struct edhoc_auth_creds* credentials) {
  return shared_credential_fetch(user_context, credentials, CLIENT_PUBLIC_KEY,
                                 ARRAY_SIZE(CLIENT_PUBLIC_KEY),
                                 CLIENT_PRIVATE_KEY,
                                 ARRAY_SIZE(CLIENT_PRIVATE_KEY), CLIENT_KID);
}

static int credential_verify(void* user_context,
                             struct edhoc_auth_creds* credentials,
                             const uint8_t** public_key_reference,
                             size_t* public_key_length) {
  int result = shared_credential_verify(
      user_context, credentials, SERVER_KID, SERVER_PUBLIC_KEY,
      ARRAY_SIZE(SERVER_PUBLIC_KEY), public_key_reference, public_key_length);

  if (result == EDHOC_SUCCESS) {
    initialize_credential_key(credentials, SERVER_PUBLIC_KEY,
                              ARRAY_SIZE(SERVER_PUBLIC_KEY), SERVER_KID);
  }

  return result;
}

static bool have_response = false;

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
  if (coap_get_data_large(received, &len, &databuf, &offset, &total)) {
    fwrite(databuf, 1, len, stdout);
    fwrite("\n", 1, 1, stdout);
  }

  return COAP_RESPONSE_OK;
}

static int run_handshake(struct edhoc_context* context, int* socket_fd,
                         struct sockaddr_in* server_address) {
  printf("Client: Waiting for server to be ready...\n");
  sleep(1);
  uint8_t message_buffer[MESSAGE_BUFFER_LENGTH] = {0};
  const size_t message_buffer_size = sizeof(message_buffer);

  size_t message1_length = 0;
  if (edhoc_message_1_compose(context, message_buffer, message_buffer_size,
                              &message1_length) != EDHOC_SUCCESS) {
    fprintf(stderr, "Failed to compose Message 1\n");
    close(*socket_fd);
    return -1;
  }

  sendto(*socket_fd, message_buffer, message1_length, 0,
         (const struct sockaddr*)server_address, sizeof(*server_address));
  printf("Client: Sent Message 1 (%zu bytes)\n", message1_length);

  socklen_t server_address_length = sizeof(*server_address);
  ssize_t message2_length =
      recvfrom(*socket_fd, message_buffer, message_buffer_size, 0,
               (struct sockaddr*)server_address, &server_address_length);
  if (message2_length < 0) {
    fprintf(stderr,
            "Failed to receive Message 2 from server. Make sure server is "
            "running!\n");
    close(*socket_fd);
    return -1;
  }
  printf("Client: Received Message 2 (%zd bytes)\n", message2_length);

  if (edhoc_message_2_process(context, message_buffer, message2_length) !=
      EDHOC_SUCCESS) {
    fprintf(stderr, "Client: Failed to process Message 2\n");
    close(*socket_fd);
    return -1;
  }

  size_t message3_length = 0;
  if (edhoc_message_3_compose(context, message_buffer, message_buffer_size,
                              &message3_length) != EDHOC_SUCCESS) {
    fprintf(stderr, "Client: Failed to compose Message 3\n");
    close(*socket_fd);
    return -1;
  }

  sendto(*socket_fd, message_buffer, message3_length, 0,
         (const struct sockaddr*)server_address, sizeof(*server_address));
  printf("Client: Sent Message 3 (%zu bytes)\n", message3_length);

  printf("Client: EDHOC Handshake Completed Successfully!");
  return EDHOC_SUCCESS;
}

static int send_message(struct edhoc_context* context, int socket_fd,
                        const struct sockaddr_in* server_address,
                        const uint8_t* plaintext_message) {
  uint8_t shared_secret[EXPORTED_SECRET_LENGTH] = {0};
  int result = edhoc_export_prk_exporter(context, PKR_OUT_LABEL, shared_secret,
                                         sizeof(shared_secret));
  if (result != EDHOC_SUCCESS) {
    fprintf(stderr, "Client: Failed to export PRK exporter\n");
    return result;
  }

  uint8_t encrypted_message[CIPHERTEXT_MAX_LENGTH] = {0};
  size_t encrypted_message_length = 0;
  const psa_status_t encrypt_status = shared_encrypt_plaintext(
      plaintext_message, strlen((const char*)plaintext_message), shared_secret,
      sizeof(shared_secret), encrypted_message, sizeof(encrypted_message),
      &encrypted_message_length);
  if (encrypt_status != PSA_SUCCESS) {
    fprintf(stderr, "Failed to encrypt the send message\n");
    return encrypt_status;
  }

  sendto(socket_fd, encrypted_message, encrypted_message_length, 0,
         (const struct sockaddr*)server_address, sizeof(*server_address));

  return EDHOC_SUCCESS;
}

/**
 * @brief Execute EDHOC client handshake and send encrypted message to server
 *
 * This function performs the complete EDHOC handshake as an initiator,
 * establishes a secure context, and sends an encrypted message to the server.
 *
 * @return EDHOC_SUCCESS on success, error code on failure
 */
int run_client() {
  // struct edhoc_context edhoc_session_context = {0};
  // const struct edhoc_credentials credentials = {
  //     .fetch = credential_fetch,
  //     .verify = credential_verify,
  // };

  // int result = edhoc_setup_context(&edhoc_session_context, &credentials);
  // if (result != EDHOC_SUCCESS) {
  //   edhoc_context_deinit(&edhoc_session_context);
  //   return result;
  // }

  // struct sockaddr_in server_address;
  // memset(&server_address, 0, sizeof(server_address));
  // server_address.sin_family = AF_INET;
  // server_address.sin_port = htons(SERVER_PORT);
  // server_address.sin_addr.s_addr = inet_addr(SERVER_URI);
  // result = run_handshake(&edhoc_session_context, &socket_fd,
  // &server_address); if (result != EDHOC_SUCCESS) {
  //   edhoc_context_deinit(&edhoc_session_context);
  //   return result;
  // }

  // result = send_message(&edhoc_session_context, socket_fd,
  // &server_address,
  //                       (const uint8_t*)"Hello Server!");
  // if (result != EDHOC_SUCCESS) {
  //   edhoc_context_deinit(&edhoc_session_context);
  //   close(socket_fd);
  //   return result;
  // }

  // close(socket_fd);
  coap_startup();
  coap_set_log_level(COAP_LOG_WARN);

  static const char CLIENT_COAP_URI[] = "coap://localhost:5683/hello";
  coap_uri_t client_uri = {0};
  int result = coap_split_uri((const unsigned char*)CLIENT_COAP_URI,
                              strlen(CLIENT_COAP_URI), &client_uri);
  if (result != 0) {
    coap_log_warn("Failed to parse uri %s\n", CLIENT_COAP_URI);
    return end_coap_session(NULL, NULL, NULL);
  }

  coap_address_t destination_address = {0};
  const uint32_t masked_protocol = 1 << client_uri.scheme;
  result = resolve_address(&client_uri.host, client_uri.port,
                           &destination_address, masked_protocol);
  if (result <= 0) {
    coap_log_warn("Failed to resolve address %*.*s\n",
                  (int)client_uri.host.length, (int)client_uri.host.length,
                  (const char*)client_uri.host.s);
    return end_coap_session(NULL, NULL, NULL);
  }
  const int is_mcast = coap_is_mcast(&destination_address);

  coap_context_t* coap_session_context = coap_new_context(NULL);
  if (!coap_session_context) {
    coap_log_emerg("cannot create libcoap context\n");
    return end_coap_session(NULL, NULL, coap_session_context);
  }

  coap_context_set_block_mode(coap_session_context,
                              USE_LIBCOAP_FOR_REQUEST_AND_SINGLE_BODY_DATA);

  coap_session_t* coap_session = NULL;
  coap_address_t* local_interface = NULL;
  coap_proto_t protocol = client_uri.scheme == COAP_URI_SCHEME_COAP_TCP
                              ? COAP_PROTO_TCP
                              : COAP_PROTO_UDP;
  coap_session = coap_new_client_session(coap_session_context, &local_interface,
                                         &destination_address, protocol);
  if (!coap_session) {
    coap_log_emerg("cannot create client session\n");
    return end_coap_session(NULL, coap_session, coap_session_context);
  }

  coap_register_response_handler(coap_session_context, response_handler);

  coap_pdu_t* message_format_header =
      coap_pdu_init(is_mcast ? COAP_MESSAGE_NON : COAP_MESSAGE_CON,
                    COAP_REQUEST_CODE_GET, coap_new_message_id(coap_session),
                    coap_session_max_pdu_size(coap_session));
  if (!message_format_header) {
    coap_log_emerg("cannot create PDU\n");
    return end_coap_session(NULL, coap_session, coap_session_context);
  }

  coap_optlist_t* optlist = NULL;
  enum { ADD_PORT_OPTION = 1, BUFFER_SIZE = 1000 };
  const unsigned char scratch[BUFFER_SIZE] = {0};
  result = coap_uri_into_options(&client_uri, &destination_address, &optlist,
                                 ADD_PORT_OPTION, scratch, sizeof(scratch));
  if (result != 0) {
    coap_log_err("Failed to create options\n");
    return end_coap_session(optlist, coap_session, coap_session_context);
  }

  if (optlist) {
    result = coap_add_optlist_pdu(message_format_header, &optlist);
    if (result != 1) {
      coap_log_err("Failed to add options to PDU\n");
      return end_coap_session(optlist, coap_session, coap_session_context);
    }
  }

  coap_show_pdu(COAP_LOG_WARN, message_format_header);

  if (coap_send(coap_session, message_format_header) == COAP_INVALID_MID) {
    coap_log_err("cannot send CoAP pdu\n");
    return end_coap_session(optlist, coap_session, coap_session_context);
  }

  enum { TIMEOUT_MS = 1000 };
  int wait_ms =
      (coap_session_get_default_leisure(coap_session).integer_part + 1) *
      TIMEOUT_MS;
  int miliseconds_spent_on_function = 0;
  while (!have_response || is_mcast) {
    miliseconds_spent_on_function =
        coap_io_process(coap_session_context, TIMEOUT_MS);
    if (miliseconds_spent_on_function < 0) {
      coap_log_err("CoAP I/O process failed\n");
      return end_coap_session(optlist, coap_session, coap_session_context);
    }
    if (wait_ms < 0 || miliseconds_spent_on_function >= wait_ms) {
      coap_log_warn("No response received within timeout\n");
      return end_coap_session(optlist, coap_session, coap_session_context);
    }
    wait_ms -= miliseconds_spent_on_function;
  }
  return 0;
}
