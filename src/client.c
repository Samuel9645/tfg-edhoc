#include "client.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <psa/crypto.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "edhoc_cipher_suite_2.h"
#include "edhoc_context.h"
#include "edhoc_setup.h"
#include "public_data.h"

static int credential_fetch(void* user_context,
                            struct edhoc_auth_creds* credentials) {
  if (credentials == NULL)
    return EDHOC_ERROR_INVALID_ARGUMENT;

  credentials->label = EDHOC_COSE_HEADER_KID;
  credentials->key_id.cred = CLIENT_PUBLIC_KEY;
  credentials->key_id.cred_len = ARRAY_SIZE(CLIENT_PUBLIC_KEY);
  credentials->key_id.cred_is_cbor = false;
  credentials->key_id.encode_type = EDHOC_ENCODE_TYPE_INTEGER;
  credentials->key_id.key_id_int = CLIENT_KID;

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

  credentials->key_id.cred = SERVER_PUBLIC_KEY;
  credentials->key_id.cred_len = ARRAY_SIZE(SERVER_PUBLIC_KEY);
  credentials->key_id.cred_is_cbor = false;

  return EDHOC_SUCCESS;
}

/**
 * @param ctx
 * @param server_ip
 * @param server_port
 * @param[out] socket_fd
 * @return
 */
static int run_handshake(struct edhoc_context* ctx, const char* server_ip,
                         const uint16_t server_port, int* socket_fd,
                         struct sockaddr_in* servaddr) {
  *socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
  if (*socket_fd < 0) {
    perror("Socket creation failed");
    return -1;
  }

  memset(servaddr, 0, sizeof(*servaddr));
  servaddr->sin_family = AF_INET;
  servaddr->sin_port = htons(server_port);
  servaddr->sin_addr.s_addr = inet_addr(server_ip);

  uint8_t msg_buf[SOCKET_MESSAGE_BUFFER_LEN] = {0};

  printf("Client: Waiting for server to be ready...\n");
  sleep(1);

  size_t msg1_len = 0;
  if (edhoc_message_1_compose(ctx, msg_buf, sizeof(msg_buf), &msg1_len) !=
      EDHOC_SUCCESS) {
    fprintf(stderr, "Failed to compose Message 1\n");
    close(*socket_fd);
    return -1;
  }

  sendto(*socket_fd, msg_buf, msg1_len, 0, (const struct sockaddr*)servaddr,
         sizeof(*servaddr));
  printf("Client: Sent Message 1 (%zu bytes)\n", msg1_len);

  socklen_t server_address_length = sizeof(*servaddr);
  ssize_t msg2_len =
      recvfrom(*socket_fd, msg_buf, sizeof(msg_buf), 0,
               (struct sockaddr*)servaddr, &server_address_length);
  if (msg2_len < 0) {
    fprintf(stderr,
            "Failed to receive Message 2 from server. Make sure server is "
            "running!\n");
    close(*socket_fd);
    return -1;
  }
  printf("Client: Received Message 2 (%zd bytes)\n", msg2_len);

  if (edhoc_message_2_process(ctx, msg_buf, msg2_len) != EDHOC_SUCCESS) {
    fprintf(stderr, "Client: Failed to process Message 2\n");
    close(*socket_fd);
    return -1;
  }

  size_t msg3_len = 0;
  if (edhoc_message_3_compose(ctx, msg_buf, sizeof(msg_buf), &msg3_len) !=
      EDHOC_SUCCESS) {
    fprintf(stderr, "Client: Failed to compose Message 3\n");
    close(*socket_fd);
    return -1;
  }

  sendto(*socket_fd, msg_buf, msg3_len, 0, (const struct sockaddr*)servaddr,
         sizeof(*servaddr));
  printf("Client: Sent Message 3 (%zu bytes)\n", msg3_len);

  printf("Client: EDHOC Handshake Completed Successfully!");
  return EDHOC_SUCCESS;
}

/**
 * @brief
 * @param plaintext
 * @param plaintext_len
 * @param key
 * @param key_len
 * @param[out] ciphertext
 * @param[out] ciphertext_len
 * @return
 */
static int encrypt(const uint8_t* plaintext, size_t plaintext_len,
                   const uint8_t* key, size_t key_len, uint8_t* ciphertext,
                   size_t ciphertext_size, size_t* ciphertext_len) {
  uint8_t nonce[13] = {0};

  // Initialize PSA Crypto
  psa_status_t status = psa_crypto_init();
  if (status != PSA_SUCCESS) {
    printf("PSA crypto init failed: %d\n", status);
    return -1;
  }

  // Set up key attributes
  psa_key_attributes_t attr = PSA_KEY_ATTRIBUTES_INIT;
  psa_set_key_usage_flags(&attr, PSA_KEY_USAGE_ENCRYPT);
  psa_set_key_algorithm(&attr, PSA_ALG_CCM);
  psa_set_key_type(&attr, PSA_KEY_TYPE_AES);
  const size_t key_bits = key_len * 8;
  psa_set_key_bits(&attr, key_bits);

  // Import the key
  psa_key_id_t key_id;
  status = psa_import_key(&attr, key, key_len, &key_id);
  if (status != PSA_SUCCESS) {
    printf("PSA key import failed: %d\n", status);
    return -1;
  }

  // Encrypt with AES-CCM
  status = psa_aead_encrypt(
      key_id, PSA_ALG_CCM, nonce, sizeof(nonce), NULL, 0,  // No AAD
      plaintext, plaintext_len, ciphertext, ciphertext_size, ciphertext_len);

  // Clean up
  psa_destroy_key(key_id);

  if (status != PSA_SUCCESS) {
    printf("PSA encryption failed: %d\n", status);
    return -1;
  }

  printf("Encryption successful: %zu bytes\n", *ciphertext_len);
  return 0;
}

int run_client() {
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

  int socket_fd = 0;
  struct sockaddr_in servaddr = {0};
  ret = run_handshake(&ctx, SERVER_URI, SERVER_PORT, &socket_fd, &servaddr);
  if (ret != EDHOC_SUCCESS) {
    edhoc_context_deinit(&ctx);
    return ret;
  }

  uint8_t secret[PKR_EXPORT_SECRET_LEN] = {0};
  ret = edhoc_export_prk_exporter(&ctx, OSCORE_EXTRACT_LABEL_MASTER_SECRET,
                                  secret, sizeof(secret));
  if (ret != EDHOC_SUCCESS) {
    fprintf(stderr, "Client: Failed to export PRK exporter\n");
    edhoc_context_deinit(&ctx);
    close(socket_fd);
    return ret;
  }

  const uint8_t* message = "Hello Server!";
  uint8_t ciphertext[CYPERTEXT_LEN] = {0};
  size_t ciphertext_len = 0;
  ret = encrypt(message, strlen(message), secret, sizeof(secret), ciphertext,
                sizeof(ciphertext), &ciphertext_len);
  if (ret != EDHOC_SUCCESS) {
    fprintf(stderr, "Client: Failed to encrypt and send message\n");
    edhoc_context_deinit(&ctx);
    close(socket_fd);
  }

  sendto(socket_fd, ciphertext, ciphertext_len, 0,
         (const struct sockaddr*)&servaddr, sizeof(servaddr));
  close(socket_fd);
  return ret;
}
