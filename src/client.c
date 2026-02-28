#include "client.h"

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

static int run_handshake(struct edhoc_context* ctx, const char* server_ip,
                         const uint16_t server_port) {
  int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sockfd < 0) {
    perror("Socket creation failed");
    return -1;
  }

  struct sockaddr_in servaddr;
  memset(&servaddr, 0, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(server_port);
  servaddr.sin_addr.s_addr = inet_addr(server_ip);

  uint8_t msg_buf[256] = {0};

  printf("Client: Waiting for server to be ready...\n");
  sleep(1);

  size_t msg1_len = 0;
  if (edhoc_message_1_compose(ctx, msg_buf, sizeof(msg_buf), &msg1_len) !=
      EDHOC_SUCCESS) {
    fprintf(stderr, "Failed to compose Message 1\n");
    close(sockfd);
    return -1;
  }

  sendto(sockfd, msg_buf, msg1_len, 0, (const struct sockaddr*)&servaddr,
         sizeof(servaddr));
  printf("Client: Sent Message 1 (%zu bytes)\n", msg1_len);

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

  if (edhoc_message_2_process(ctx, msg_buf, msg2_len) != EDHOC_SUCCESS) {
    fprintf(stderr, "Client: Failed to process Message 2\n");
    close(sockfd);
    return -1;
  }

  size_t msg3_len = 0;
  if (edhoc_message_3_compose(ctx, msg_buf, sizeof(msg_buf), &msg3_len) !=
      EDHOC_SUCCESS) {
    fprintf(stderr, "Client: Failed to compose Message 3\n");
    close(sockfd);
    return -1;
  }

  sendto(sockfd, msg_buf, msg3_len, 0, (const struct sockaddr*)&servaddr,
         sizeof(servaddr));
  printf("Client: Sent Message 3 (%zu bytes)\n", msg3_len);

  printf("Client: EDHOC Handshake Completed Successfully!");
  close(sockfd);

  return EDHOC_SUCCESS;
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

  ret = run_handshake(&ctx, SERVER_URI, SERVER_PORT);
  edhoc_context_deinit(&ctx);
  return ret;
}
