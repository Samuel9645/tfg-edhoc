#include <arpa/inet.h>
#include <netinet/in.h>
#include <psa/crypto.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 9000

#include "edhoc.h"
#include "edhoc_cipher_suite_2.h"
#include "edhoc_context.h"

static const int32_t SERVER_KID = 20;
static const int32_t CLIENT_KID = 18;
static const int32_t CONNECTION_ID = 21;  // Example connection ID

// Test vectors from RFC 9529 - Server keys
static const uint8_t server_private_key[] = {
    0xbc, 0xfc, 0x08, 0x24, 0x79, 0xa1, 0x90, 0x98, 0xf4, 0xd4, 0x9a,
    0xf5, 0x0b, 0xbe, 0x12, 0x92, 0xbd, 0xda, 0x1a, 0x8a, 0x92, 0xb1,
    0xcd, 0x75, 0xff, 0x42, 0x0d, 0x72, 0x0f, 0xc1, 0x94, 0xff};

static const uint8_t server_public_key[] = {
    0x04, 0x78, 0x28, 0xdd, 0x49, 0x29, 0xc8, 0x90, 0x84, 0x36, 0xa6,
    0xaf, 0xeb, 0x94, 0x9f, 0xa0, 0x94, 0xaf, 0xcd, 0xc7, 0x41, 0x9f,
    0x51, 0x9d, 0x7f, 0x81, 0x5b, 0x38, 0x20, 0x2d, 0xa7, 0x87, 0x25,
    0x84, 0x75, 0x65, 0x42, 0x03, 0x40, 0x6b, 0x7d, 0x06, 0xc2, 0xfb,
    0xec, 0x31, 0x9f, 0xab, 0x95, 0x3b, 0x87, 0xeb, 0xe2, 0xce, 0xdb,
    0xc7, 0xe2, 0x93, 0x59, 0x49, 0x29, 0x62, 0x24, 0x4d, 0x32};

// Client public key (for verification)
static const uint8_t client_public_key[] = {
    0x04, 0x8d, 0x26, 0x81, 0xba, 0xc7, 0x3b, 0xa5, 0xcf, 0x68, 0xd8,
    0xca, 0xf8, 0x1e, 0x41, 0xfe, 0x22, 0xaf, 0xa6, 0x5e, 0x69, 0x31,
    0x35, 0x84, 0x92, 0xda, 0xc5, 0x85, 0x70, 0xd9, 0xfd, 0x4c, 0xe5,
    0x1a, 0xcc, 0xc3, 0x67, 0x13, 0xc6, 0x0d, 0x21, 0x1d, 0xcb, 0x43,
    0xdf, 0x51, 0x25, 0xef, 0x70, 0xb4, 0x13, 0xbc, 0x3a, 0x2c, 0x8a,
    0xcf, 0xb9, 0xae, 0xb5, 0x3a, 0xe4, 0x81, 0x39, 0x56, 0x71};

int credential_fetch(void* user_context, struct edhoc_auth_creds* credentials) {
  (void)user_context;
  if (credentials == NULL)
    return EDHOC_ERROR_INVALID_ARGUMENT;

  credentials->label = EDHOC_COSE_HEADER_KID;

  // Provide our RAW public key to the library
  credentials->key_id.cred = server_public_key;
  credentials->key_id.cred_len = sizeof(server_public_key);
  credentials->key_id.cred_is_cbor = false;

  credentials->key_id.encode_type = EDHOC_ENCODE_TYPE_INTEGER;
  credentials->key_id.key_id_int = SERVER_KID;

  // Import private key for signature operations (ECDSA for Message 2+
  // authentication)
  int ret = edhoc_cipher_suite_2_key_import(
      user_context, EDHOC_KT_SIGNATURE, server_private_key,
      sizeof(server_private_key), credentials->priv_key_id);

  if (ret != EDHOC_SUCCESS) {
    return EDHOC_ERROR_CREDENTIALS_FAILURE;
  }

  return EDHOC_SUCCESS;
}

int credential_verify(void* user_context, struct edhoc_auth_creds* credentials,
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

  *public_key_reference = client_public_key;
  *public_key_length = sizeof(client_public_key);

  credentials->key_id.cred = server_public_key;
  credentials->key_id.cred_len = sizeof(server_public_key);
  credentials->key_id.cred_is_cbor = false;

  return EDHOC_SUCCESS;
}

int main() {
  // Initialize PSA Crypto subsystem
  psa_status_t psa_status = psa_crypto_init();
  if (psa_status != PSA_SUCCESS) {
    fprintf(stderr, "Failed to initialize PSA crypto: %d\n", psa_status);
    return psa_status;
  }

  // Initialize context
  struct edhoc_context ctx = {0};
  int ret = edhoc_context_init(&ctx);
  if (ret != EDHOC_SUCCESS) {
    fprintf(stderr, "Failed to initialize context: %d\n", ret);
    return ret;
  }

  // Configure cipher suite and methods
  const enum edhoc_method METHODS[] = {EDHOC_METHOD_0};
  ret = edhoc_set_methods(&ctx, METHODS, ARRAY_SIZE(METHODS));
  if (ret != EDHOC_SUCCESS) {
    fprintf(stderr, "Failed to set methods: %d\n", ret);
    edhoc_context_deinit(&ctx);
    return ret;
  }

  /**
   * @link https://datatracker.ietf.org/doc/html/rfc9528/#name-cipher-suites
   * @brief Defines a series of parameters to secure the communications.
   * 0-3 are specific for IoT devices
   *
   * @link https://datatracker.ietf.org/doc/html/rfc9528/#suites-registry
   * Cipher suites and methods available
   *
   * @link
   * https://www.geeksforgeeks.org/computer-networks/what-is-nonce-in-cryptography/
   * iv_length
   */
  const struct edhoc_cipher_suite cipher_suite_2 = {
      .value = 2,
      .aead_key_length = 16,
      .aead_tag_length = 8,
      .aead_iv_length = 13,
      .hash_length = 32,
      .mac_length = 32,
      .ecc_key_length = 32,
      .ecc_sign_length = 64,
  };
  const struct edhoc_cipher_suite cipher_suites[] = {cipher_suite_2};
  ret = edhoc_set_cipher_suites(&ctx, cipher_suites, ARRAY_SIZE(cipher_suites));
  if (ret != EDHOC_SUCCESS) {
    fprintf(stderr, "Failed to set cipher suites: %d\n", ret);
    edhoc_context_deinit(&ctx);
    return ret;
  }

  /**
   * @link
   * https://datatracker.ietf.org/doc/html/rfc9528/#name-connection-identifiers
   * @brief Compact tags that allows two devices to keep track of the current
   * state of the protocol, usefull when communicating over UDP or CoAP
   * (protocols that can be used without conection).
   *
   * Those tags can be converted to OSCORE Recipient ID and Sender ID, a good
   * connection id must be pick so that when converting both identifiers to
   * OSCORE identifiers they must be diferent
   */
  const struct edhoc_connection_id connection_id = {
      .encode_type = EDHOC_CID_TYPE_ONE_BYTE_INTEGER,
      .int_value = CONNECTION_ID,
      .bstr_length = 0};
  ret = edhoc_set_connection_id(&ctx, &connection_id);
  if (ret != EDHOC_SUCCESS) {
    fprintf(stderr, "Failed to set connection ID: %d\n", ret);
    edhoc_context_deinit(&ctx);
    return ret;
  }

  // Bind crypto operations
  ret = edhoc_bind_keys(&ctx, edhoc_cipher_suite_2_get_keys());
  if (ret != EDHOC_SUCCESS) {
    fprintf(stderr, "Failed to bind keys: %d\n", ret);
    edhoc_context_deinit(&ctx);
    return ret;
  }

  ret = edhoc_bind_crypto(&ctx, edhoc_cipher_suite_2_get_crypto());
  if (ret != EDHOC_SUCCESS) {
    fprintf(stderr, "Failed to bind crypto: %d\n", ret);
    edhoc_context_deinit(&ctx);
    return ret;
  }

  // Bind credentials
  // This registers our fetch and verify callbacks with the context
  // - fetch: Called when we need to provide our credentials
  // - verify: Called when we receive and need to validate peer's credentials
  const struct edhoc_credentials credentials = {.fetch = credential_fetch,
                                                .verify = credential_verify};

  ret = edhoc_bind_credentials(&ctx, &credentials);
  if (ret != EDHOC_SUCCESS) {
    fprintf(stderr, "Failed to bind credentials: %d\n", ret);
    edhoc_context_deinit(&ctx);
    return ret;
  }

  // ========================================================================
  // UDP SOCKET SETUP (SERVER)
  // ========================================================================
  int sockfd;
  struct sockaddr_in servaddr, cliaddr;

  if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    perror("Socket creation failed");
    return -1;
  }

  memset(&servaddr, 0, sizeof(servaddr));
  memset(&cliaddr, 0, sizeof(cliaddr));

  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = INADDR_ANY;
  servaddr.sin_port = htons(PORT);

  if (bind(sockfd, (const struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
    perror("Bind failed");
    return -1;
  }

  // ========================================================================
  // MESSAGE PROTOCOL FLOW
  // ========================================================================
  uint8_t msg_buf[256] = {0};
  socklen_t len = sizeof(cliaddr);

  printf("Server listening on port %d...\n", PORT);

  // 1. Receive Message 1 from Client
  ssize_t msg1_len = recvfrom(sockfd, msg_buf, sizeof(msg_buf), 0,
                              (struct sockaddr*)&cliaddr, &len);
  printf("Received Message 1 (%zd bytes)\n", msg1_len);

  if (edhoc_message_1_process(&ctx, msg_buf, msg1_len) != EDHOC_SUCCESS) {
    printf("Failed to process Message 1\n");
    return -1;
  }

  // 2. Compose Message 2
  size_t msg2_len = 0;
  if (edhoc_message_2_compose(&ctx, msg_buf, sizeof(msg_buf), &msg2_len) !=
      EDHOC_SUCCESS) {
    printf("Failed to compose Message 2\n");
    return -1;
  }

  // Send Message 2 back to Client
  sendto(sockfd, msg_buf, msg2_len, 0, (const struct sockaddr*)&cliaddr, len);
  printf("Sent Message 2 (%zu bytes)\n", msg2_len);

  // 3. Receive Message 3 from Client
  ssize_t msg3_len = recvfrom(sockfd, msg_buf, sizeof(msg_buf), 0,
                              (struct sockaddr*)&cliaddr, &len);
  printf("Received Message 3 (%zd bytes)\n", msg3_len);

  if (edhoc_message_3_process(&ctx, msg_buf, msg3_len) != EDHOC_SUCCESS) {
    printf("Failed to process Message 3\n");
    return -1;
  }

  printf("Server: EDHOC Handshake Completed Successfully!\n");
  close(sockfd);
  edhoc_context_deinit(&ctx);
  return EDHOC_SUCCESS;
}
