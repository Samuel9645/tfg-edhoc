#include "server.h"

static int credential_fetch(void* user_context,
                            struct edhoc_auth_creds* credentials) {
  (void)user_context;
  if (credentials == NULL)
    return EDHOC_ERROR_INVALID_ARGUMENT;

  credentials->label = EDHOC_COSE_HEADER_KID;
  credentials->key_id.cred = SERVER_PUBLIC_KEY;
  credentials->key_id.cred_len = sizeof(SERVER_PUBLIC_KEY);
  credentials->key_id.cred_is_cbor = false;
  credentials->key_id.encode_type = EDHOC_ENCODE_TYPE_INTEGER;
  credentials->key_id.key_id_int = SERVER_KID;

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

  credentials->key_id.cred = CLIENT_PUBLIC_KEY;
  credentials->key_id.cred_len = ARRAY_SIZE(CLIENT_PUBLIC_KEY);
  credentials->key_id.cred_is_cbor = false;

  return EDHOC_SUCCESS;
}

static int run_handshake(struct edhoc_context* ctx, int* socket_fd,
                         struct sockaddr_in* server_address,
                         struct sockaddr_in* client_address) {
  socklen_t client_address_len = sizeof(*client_address);

  printf("Server listening on port %d...\n", ntohs(server_address->sin_port));

  uint8_t msg_buf[SOCKET_MESSAGE_BUFFER_LEN] = {0};
  const size_t msg_buf_size = sizeof(msg_buf);

  ssize_t msg1_len =
      recvfrom(*socket_fd, msg_buf, msg_buf_size, 0,
               (struct sockaddr*)client_address, &client_address_len);
  printf("Received Message 1 (%zd bytes)\n", msg1_len);

  if (edhoc_message_1_process(ctx, msg_buf, msg1_len) != EDHOC_SUCCESS) {
    fprintf(stderr, "Failed to process Message 1\n");
    return -1;
  }

  size_t msg2_len = 0;
  if (edhoc_message_2_compose(ctx, msg_buf, msg_buf_size, &msg2_len) !=
      EDHOC_SUCCESS) {
    fprintf(stderr, "Failed to compose Message 2\n");
    return -1;
  }

  sendto(*socket_fd, msg_buf, msg2_len, 0,
         (const struct sockaddr*)client_address, client_address_len);
  printf("Sent Message 2 (%zu bytes)\n", msg2_len);

  ssize_t msg3_len =
      recvfrom(*socket_fd, msg_buf, msg_buf_size, 0,
               (struct sockaddr*)client_address, &client_address_len);
  printf("Received Message 3 (%zd bytes)\n", msg3_len);

  if (edhoc_message_3_process(ctx, msg_buf, msg3_len) != EDHOC_SUCCESS) {
    fprintf(stderr, "Failed to process Message 3\n");
    return -1;
  }

  printf("Server: EDHOC Handshake Completed Successfully!\n");

  return EDHOC_SUCCESS;
}

/**
 * @param ciphertext
 * @param ciphertext_len
 * @param key
 * @param key_len
 * @param[out] plaintext
 * @param[out] plaintext_len
 * @return
 */
static int decrypt_cyphertext(const uint8_t* ciphertext, size_t ciphertext_len,
                              const uint8_t* key, size_t key_len,
                              uint8_t* plaintext, size_t plaintext_size,
                              size_t* plaintext_len) {
  psa_key_id_t key_id;
  psa_status_t status =
      setup_psa_crypto(PSA_KEY_USAGE_DECRYPT, key, key_len, &key_id);
  if (status != PSA_SUCCESS) {
    return status;
  }

  uint8_t nonce[13] = {0};
  // Decrypt with AES-CCM
  status = psa_aead_decrypt(
      key_id, PSA_ALG_CCM, nonce, sizeof(nonce), NULL, 0,  // No AAD
      ciphertext, ciphertext_len, plaintext, plaintext_size, plaintext_len);
  // Clean up
  return psa_destroy_key(key_id);
}

/**
 * @brief
 * @param ctx
 * @param socket_fd
 * @param client_address
 * @param recieved_message_size
 * @param[out] recieved_message
 * @param[out] recieved_message_len
 * @return
 */
static int recieve_message(struct edhoc_context* ctx, int socket_fd,
                           struct sockaddr_in* client_address,
                           size_t recieved_message_size,
                           uint8_t* recieved_message,
                           size_t* recieved_message_len) {
  uint8_t secret[PKR_EXPORT_SECRET_LEN] = {0};
  int ret =
      edhoc_export_prk_exporter(ctx, PKR_OUT_LABEL, secret, sizeof(secret));
  if (ret != EDHOC_SUCCESS) {
    fprintf(stderr, "Server: Failed to export PRK exporter\n");
    return ret;
  }

  uint8_t ciphertext[CYPERTEXT_LEN] = {0};
  socklen_t client_address_len = sizeof(*client_address);
  ret = recvfrom(socket_fd, ciphertext, sizeof(ciphertext), 0,
                 (struct sockaddr*)client_address, &client_address_len);
  if (ret < 0) {
    fprintf(stderr, "Failed to receive ciphertext\n");
    return -1;
  }

  size_t ciphertext_len = (size_t)ret;
  // printf("Server: Received ciphertext from client: %.*s\n", (int)ciphertext_len,
  //        ciphertext);
  const psa_status_t decrypt_status = decrypt_cyphertext(
      ciphertext, ciphertext_len, secret, sizeof(secret), recieved_message,
      recieved_message_size, recieved_message_len);
  if (decrypt_status != PSA_SUCCESS) {
    fprintf(stderr, "Failed to decrypt ciphertext\n");
  }
  return EDHOC_SUCCESS;
}

int run_server() {
  struct edhoc_context ctx = {0};
  const struct edhoc_credentials credentials = {
      .fetch = credential_fetch,
      .verify = credential_verify,
  };

  int ret = edhoc_setup_context(&ctx, &credentials);
  if (ret != EDHOC_SUCCESS) {
    edhoc_context_deinit(&ctx);
    return ret;
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

  ret = run_handshake(&ctx, &socket_fd, &server_address, &client_address);
  if (ret != EDHOC_SUCCESS) {
    fprintf(stderr, "Failed to run handshake\n");
    close(socket_fd);
    edhoc_context_deinit(&ctx);
    return ret;
  }

  u_int8_t message[SOCKET_MESSAGE_BUFFER_LEN] = {0};
  size_t message_len = 0;
  ret = recieve_message(&ctx, socket_fd, &client_address, sizeof(message),
                        message, &message_len);
  if (ret != EDHOC_SUCCESS) {
    close(socket_fd);
    edhoc_context_deinit(&ctx);
    return ret;
  }
  printf("Server: Received message from client: %.*s\n", (int)message_len,
         message);

  close(socket_fd);
  edhoc_context_deinit(&ctx);

  return 0;
}
