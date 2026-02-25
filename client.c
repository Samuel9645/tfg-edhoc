#include <arpa/inet.h>
#include <netinet/in.h>
#include <psa/crypto.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>

#define PORT 9000

#include "edhoc.h"
#include "edhoc_cipher_suite_2.h"
#include "edhoc_context.h"

static const int32_t SERVER_KID = 20;
static const int32_t CLIENT_KID = 18;
static const int32_t CONNECTION_ID = 21;

// Test vectors from RFC 9529 - Client keys
static const uint8_t client_private_key[] = {
    0x97, 0x45, 0x6c, 0x36, 0x90, 0x89, 0x0e, 0xc0, 0xa5, 0x06, 0x01,
    0x69, 0xf8, 0xc0, 0xb1, 0x80, 0xf8, 0xe3, 0xd8, 0x12, 0x3f, 0xf7,
    0xd0, 0x92, 0x3f, 0x34, 0x77, 0xb6, 0x9a, 0xea, 0x21, 0x0f};

static const uint8_t client_public_key[] = {
    0x04, 0x8d, 0x26, 0x81, 0xba, 0xc7, 0x3b, 0xa5, 0xcf, 0x68, 0xd8,
    0xca, 0xf8, 0x1e, 0x41, 0xfe, 0x22, 0xaf, 0xa6, 0x5e, 0x69, 0x31,
    0x35, 0x84, 0x92, 0xda, 0xc5, 0x85, 0x70, 0xd9, 0xfd, 0x4c, 0xe5,
    0x1a, 0xcc, 0xc3, 0x67, 0x13, 0xc6, 0x0d, 0x21, 0x1d, 0xcb, 0x43,
    0xdf, 0x51, 0x25, 0xef, 0x70, 0xb4, 0x13, 0xbc, 0x3a, 0x2c, 0x8a,
    0xcf, 0xb9, 0xae, 0xb5, 0x3a, 0xe4, 0x81, 0x39, 0x56, 0x71};

// Test vectors from RFC 9529 - Server keys (different key pair)
static const uint8_t server_public_key[] = {
    0x04, 0x78, 0x28, 0xdd, 0x49, 0x29, 0xc8, 0x90, 0x84, 0x36, 0xa6,
    0xaf, 0xeb, 0x94, 0x9f, 0xa0, 0x94, 0xaf, 0xcd, 0xc7, 0x41, 0x9f,
    0x51, 0x9d, 0x7f, 0x81, 0x5b, 0x38, 0x20, 0x2d, 0xa7, 0x87, 0x25,
    0x84, 0x75, 0x65, 0x42, 0x03, 0x40, 0x6b, 0x7d, 0x06, 0xc2, 0xfb,
    0xec, 0x31, 0x9f, 0xab, 0x95, 0x3b, 0x87, 0xeb, 0xe2, 0xce, 0xdb,
    0xc7, 0xe2, 0x93, 0x59, 0x49, 0x29, 0x62, 0x24, 0x4d, 0x32};

int credential_fetch(void* user_context, struct edhoc_auth_creds* credentials) {
  if (credentials == NULL)
    return EDHOC_ERROR_INVALID_ARGUMENT;

  credentials->label = EDHOC_COSE_HEADER_KID;

  // Provide our RAW public key to the library
  credentials->key_id.cred = client_public_key;
  credentials->key_id.cred_len = sizeof(client_public_key);
  credentials->key_id.cred_is_cbor = false;

  credentials->key_id.encode_type = EDHOC_ENCODE_TYPE_INTEGER;
  credentials->key_id.key_id_int = CLIENT_KID;

  // Import private key for signature operations (ECDSA for Message 2+
  // authentication)
  int ret = edhoc_cipher_suite_2_key_import(
      user_context, EDHOC_KT_SIGNATURE, client_private_key,
      sizeof(client_private_key), credentials->priv_key_id);

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

  if (SERVER_KID != credentials->key_id.key_id_int)
    return EDHOC_ERROR_CREDENTIALS_FAILURE;

  *public_key_reference = server_public_key;
  *public_key_length = sizeof(server_public_key);

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
  // UDP SOCKET SETUP (CLIENT)
  // ========================================================================
  int sockfd;
  struct sockaddr_in servaddr;

  if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    perror("Socket creation failed");
    return -1;
  }

  memset(&servaddr, 0, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(PORT);
  servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");

  // Set socket timeout to 5 seconds
  struct timeval tv;
  tv.tv_sec = 5;
  tv.tv_usec = 0;
  setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);

  // ========================================================================
  // MESSAGE PROTOCOL FLOW
  // ========================================================================
  uint8_t msg_buf[256] = {0};

  printf("Client: Waiting for server to be ready...\n");
  sleep(1);  // Give server time to start

  // 1. Compose Message 1
  size_t msg1_len = 0;
  if (edhoc_message_1_compose(&ctx, msg_buf, sizeof(msg_buf), &msg1_len) !=
      EDHOC_SUCCESS) {
    fprintf(stderr, "Failed to compose Message 1\n");
    close(sockfd);
    return -1;
  }

  // Send Message 1 to Server
  sendto(sockfd, msg_buf, msg1_len, 0, (const struct sockaddr*)&servaddr,
         sizeof(servaddr));
  printf("Client: Sent Message 1 (%zu bytes)\n", msg1_len);

  // 2. Receive Message 2 from Server
  socklen_t len = sizeof(servaddr);
  ssize_t msg2_len = recvfrom(sockfd, msg_buf, sizeof(msg_buf), 0,
                              (struct sockaddr*)&servaddr, &len);
  if (msg2_len < 0) {
    fprintf(stderr,
            "Failed to receive Message 2 from server. Make sure server is "
            "running!\n");
    close(sockfd);
    return -1;
  }
  printf("Client: Received Message 2 (%zd bytes)\n", msg2_len);

  if (edhoc_message_2_process(&ctx, msg_buf, msg2_len) != EDHOC_SUCCESS) {
    fprintf(stderr, "Client: Failed to process Message 2\n");
    close(sockfd);
    return -1;
  }

  // 3. Compose Message 3
  size_t msg3_len = 0;
  if (edhoc_message_3_compose(&ctx, msg_buf, sizeof(msg_buf), &msg3_len) !=
      EDHOC_SUCCESS) {
    fprintf(stderr, "Client: Failed to compose Message 3\n");
    close(sockfd);
    return -1;
  }

  // Send Message 3 to Server
  sendto(sockfd, msg_buf, msg3_len, 0, (const struct sockaddr*)&servaddr,
         sizeof(servaddr));
  printf("Client: Sent Message 3 (%zu bytes)\n", msg3_len);

  printf("\n*** Client: EDHOC Handshake Completed Successfully! ***\n");
  close(sockfd);
  edhoc_context_deinit(&ctx);
  return EDHOC_SUCCESS;
}
