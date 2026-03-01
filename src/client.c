#include "client.h"

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
  struct edhoc_context context = {0};
  const struct edhoc_credentials credentials = {
      .fetch = credential_fetch,
      .verify = credential_verify,
  };

  int result = edhoc_setup_context(&context, &credentials);
  if (result != EDHOC_SUCCESS) {
    edhoc_context_deinit(&context);
    return result;
  }

  int socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
  if (socket_fd < 0) {
    perror("Socket creation failed");
    return -1;
  }
  struct sockaddr_in server_address;
  memset(&server_address, 0, sizeof(server_address));
  server_address.sin_family = AF_INET;
  server_address.sin_port = htons(SERVER_PORT);
  server_address.sin_addr.s_addr = inet_addr(SERVER_URI);
  result = run_handshake(&context, &socket_fd, &server_address);
  if (result != EDHOC_SUCCESS) {
    edhoc_context_deinit(&context);
    return result;
  }

  result = send_message(&context, socket_fd, &server_address,
                        (const uint8_t*)"Hello Server!");
  if (result != EDHOC_SUCCESS) {
    edhoc_context_deinit(&context);
    close(socket_fd);
    return result;
  }

  close(socket_fd);
  return 0;
}
