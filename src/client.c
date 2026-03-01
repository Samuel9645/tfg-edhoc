#include "client.h"

static int credential_fetch(void* user_context,
                            struct edhoc_auth_creds* credentials) {
  if (credentials == NULL)
    return EDHOC_ERROR_INVALID_ARGUMENT;

  initialize_credential_key(credentials, CLIENT_PUBLIC_KEY,
                            ARRAY_SIZE(CLIENT_PUBLIC_KEY), CLIENT_KID);

  int ret = edhoc_cipher_suite_2_key_import(
      user_context, EDHOC_KT_SIGNATURE, CLIENT_PRIVATE_KEY,
      ARRAY_SIZE(CLIENT_PRIVATE_KEY), credentials->priv_key_id);

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

  if (SERVER_KID != credentials->key_id.key_id_int)
    return EDHOC_ERROR_CREDENTIALS_FAILURE;

  *public_key_reference = SERVER_PUBLIC_KEY;
  *public_key_length = ARRAY_SIZE(SERVER_PUBLIC_KEY);

  initialize_credential_key(credentials, SERVER_PUBLIC_KEY,
                            ARRAY_SIZE(SERVER_PUBLIC_KEY), SERVER_KID);

  return EDHOC_SUCCESS;
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

/**
 * @brief Encrypt plaintext message using AES-CCM
 * @param plaintext_buffer Buffer containing plaintext to encrypt
 * @param plaintext_length Length of plaintext
 * @param key_material Encryption key material
 * @param key_length Key length in bytes
 * @param[out] ciphertext_buffer Output buffer for encrypted data
 * @param ciphertext_buffer_size Size of output buffer
 * @param[out] ciphertext_output_length Actual length of ciphertext
 * @return Status code
 */
static int encrypt_plaintext(const uint8_t* plaintext_buffer,
                             size_t plaintext_length,
                             const uint8_t* key_material, size_t key_length,
                             uint8_t* ciphertext_buffer,
                             size_t ciphertext_buffer_size,
                             size_t* ciphertext_output_length) {
  psa_key_id_t key_id;
  psa_status_t status = setup_psa_crypto(PSA_KEY_USAGE_ENCRYPT, key_material,
                                         key_length, &key_id);
  if (status != PSA_SUCCESS) {
    return status;
  }

  uint8_t nonce[NONCE_LENGTH_BYTES] = {0};
  // Encrypt with AES-CCM
  status =
      psa_aead_encrypt(key_id, PSA_ALG_CCM, nonce, sizeof(nonce), NULL,
                       0,  // No AAD
                       plaintext_buffer, plaintext_length, ciphertext_buffer,
                       ciphertext_buffer_size, ciphertext_output_length);

  // Clean up
  return psa_destroy_key(key_id);
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
  const psa_status_t encrypt_status = encrypt_plaintext(
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
