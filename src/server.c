#include "server.h"

#include <arpa/inet.h>
#include <netinet/in.h>
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

static int run_handshake(struct edhoc_context* ctx, const char* server_ip,
                         const uint16_t port) {
  int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sockfd < 0) {
    perror("Socket creation failed");
    return -1;
  }

  struct sockaddr_in servaddr;
  struct sockaddr_in cliaddr;
  memset(&servaddr, 0, sizeof(servaddr));
  memset(&cliaddr, 0, sizeof(cliaddr));

  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = inet_addr(server_ip);
  servaddr.sin_port = htons(port);

  if (bind(sockfd, (const struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
    perror("Bind failed");
    close(sockfd);
    return -1;
  }

  uint8_t msg_buf[256] = {0};
  socklen_t len = sizeof(cliaddr);

  printf("Server listening on port %d...\n", port);

  ssize_t msg1_len = recvfrom(sockfd, msg_buf, sizeof(msg_buf), 0,
                              (struct sockaddr*)&cliaddr, &len);
  printf("Received Message 1 (%zd bytes)\n", msg1_len);

  if (edhoc_message_1_process(ctx, msg_buf, msg1_len) != EDHOC_SUCCESS) {
    fprintf(stderr, "Failed to process Message 1\n");
    close(sockfd);
    return -1;
  }

  size_t msg2_len = 0;
  if (edhoc_message_2_compose(ctx, msg_buf, sizeof(msg_buf), &msg2_len) !=
      EDHOC_SUCCESS) {
    fprintf(stderr, "Failed to compose Message 2\n");
    close(sockfd);
    return -1;
  }

  sendto(sockfd, msg_buf, msg2_len, 0, (const struct sockaddr*)&cliaddr, len);
  printf("Sent Message 2 (%zu bytes)\n", msg2_len);

  ssize_t msg3_len = recvfrom(sockfd, msg_buf, sizeof(msg_buf), 0,
                              (struct sockaddr*)&cliaddr, &len);
  printf("Received Message 3 (%zd bytes)\n", msg3_len);

  if (edhoc_message_3_process(ctx, msg_buf, msg3_len) != EDHOC_SUCCESS) {
    fprintf(stderr, "Failed to process Message 3\n");
    close(sockfd);
    return -1;
  }

  printf("Server: EDHOC Handshake Completed Successfully!\n");
  close(sockfd);

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

  ret = run_handshake(&ctx, SERVER_URI, SERVER_PORT);
  edhoc_context_deinit(&ctx);
  return ret;
}
