#include "server.h"

static int credential_fetch(void* user_context,
                            struct edhoc_auth_creds* credentials) {
  (void)user_context;
  if (credentials == NULL)
    return EDHOC_ERROR_INVALID_ARGUMENT;

  initialize_credential_key(credentials, SERVER_PUBLIC_KEY,
                            sizeof(SERVER_PUBLIC_KEY), SERVER_KID);

  int ret = edhoc_cipher_suite_2_key_import(
      user_context, EDHOC_KT_SIGNATURE, SERVER_PRIVATE_KEY,
      sizeof(SERVER_PRIVATE_KEY), credentials->priv_key_id);

  if (ret != EDHOC_SUCCESS) {
    return EDHOC_ERROR_CREDENTIALS_FAILURE;
  }

  return EDHOC_SUCCESS;
}

static int credential_verify(void* user_context,
                             struct edhoc_auth_creds* credentials,
                             const uint8_t** public_key_reference,
                             size_t* public_key_length) {
  (void)user_context;
  if (NULL == credentials)
    return EDHOC_ERROR_INVALID_ARGUMENT;

  if (EDHOC_COSE_HEADER_KID != credentials->label)
    return EDHOC_ERROR_CREDENTIALS_FAILURE;

  if (EDHOC_ENCODE_TYPE_INTEGER != credentials->key_id.encode_type)
    return EDHOC_ERROR_CREDENTIALS_FAILURE;

  if (CLIENT_KID != credentials->key_id.key_id_int)
    return EDHOC_ERROR_CREDENTIALS_FAILURE;

  *public_key_reference = CLIENT_PUBLIC_KEY;
  *public_key_length = ARRAY_SIZE(CLIENT_PUBLIC_KEY);

  initialize_credential_key(credentials, CLIENT_PUBLIC_KEY,
                            ARRAY_SIZE(CLIENT_PUBLIC_KEY), CLIENT_KID);

  return EDHOC_SUCCESS;
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

  if (edhoc_message_1_process(context, message_buffer, message1_length) != EDHOC_SUCCESS) {
    fprintf(stderr, "Failed to process Message 1\n");
    return -1;
  }

  size_t message2_length = 0;
  if (edhoc_message_2_compose(context, message_buffer, message_buffer_size, &message2_length) !=
      EDHOC_SUCCESS) {
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

  if (edhoc_message_3_process(context, message_buffer, message3_length) != EDHOC_SUCCESS) {
    fprintf(stderr, "Failed to process Message 3\n");
    return -1;
  }

  printf("Server: EDHOC Handshake Completed Successfully!\n");

  return EDHOC_SUCCESS;
}

/**
 * @brief Decrypt ciphertext using AES-CCM
 * @param ciphertext_buffer Buffer containing ciphertext to decrypt
 * @param ciphertext_length Length of ciphertext
 * @param key_material Decryption key material
 * @param key_length Key length in bytes
 * @param[out] plaintext_buffer Output buffer for decrypted data
 * @param plaintext_buffer_size Size of output buffer
 * @param[out] plaintext_output_length Actual length of plaintext
 * @return Status code
 */
static int decrypt_ciphertext(const uint8_t* ciphertext_buffer, size_t ciphertext_length,
                               const uint8_t* key_material, size_t key_length,
                               uint8_t* plaintext_buffer, size_t plaintext_buffer_size,
                               size_t* plaintext_output_length) {
  psa_key_id_t key_id;
  psa_status_t status =
      setup_psa_crypto(PSA_KEY_USAGE_DECRYPT, key_material, key_length, &key_id);
  if (status != PSA_SUCCESS) {
    return status;
  }

  uint8_t nonce[NONCE_LENGTH_BYTES] = {0};
  // Decrypt with AES-CCM
  status = psa_aead_decrypt(
      key_id, PSA_ALG_CCM, nonce, sizeof(nonce), NULL, 0,  // No AAD
      ciphertext_buffer, ciphertext_length, plaintext_buffer, plaintext_buffer_size, plaintext_output_length);
  // Clean up
  return psa_destroy_key(key_id);
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
  int result =
      edhoc_export_prk_exporter(context, PKR_OUT_LABEL, shared_secret, sizeof(shared_secret));
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

  size_t encrypted_message_length = (size_t)result;
  const psa_status_t decrypt_status = decrypt_ciphertext(
      encrypted_message, encrypted_message_length, shared_secret, sizeof(shared_secret),
      received_message_buffer, received_message_buffer_size, received_message_length);
  if (decrypt_status != PSA_SUCCESS) {
    fprintf(stderr, "Failed to decrypt ciphertext\n");
  }
  return EDHOC_SUCCESS;
}

int run_server() {
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
    fprintf(stderr, "Socket creation failed\n");
    return -1;
  }

  struct sockaddr_in server_address;
  struct sockaddr_in client_address;
  memset(&server_address, 0, sizeof(server_address));
  memset(&client_address, 0, sizeof(client_address));

  server_address.sin_addr.s_addr = htonl(INADDR_ANY);
  server_address.sin_port = htons(SERVER_PORT);
  server_address.sin_family = AF_INET;

  if (bind(socket_fd, (const struct sockaddr*)&server_address,
           sizeof(server_address)) < 0) {
    fprintf(stderr, "Bind failed\n");
    close(socket_fd);
    return -1;
  }

  result = run_handshake(&context, &socket_fd, &server_address, &client_address);
  if (result != EDHOC_SUCCESS) {
    fprintf(stderr, "Failed to run handshake\n");
    close(socket_fd);
    edhoc_context_deinit(&context);
    return result;
  }

  uint8_t received_message[MESSAGE_BUFFER_LENGTH] = {0};
  size_t received_message_length = 0;
  result = receive_message(&context, socket_fd, &client_address, sizeof(received_message),
                           received_message, &received_message_length);
  if (result != EDHOC_SUCCESS) {
    close(socket_fd);
    edhoc_context_deinit(&context);
    return result;
  }
  printf("Server: Received message from client: %.*s\n", (int)received_message_length,
         received_message);

  close(socket_fd);
  edhoc_context_deinit(&context);

  return 0;
}
