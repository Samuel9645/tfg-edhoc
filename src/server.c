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

  int sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sock_fd < 0) {
    perror("Socket creation failed");
    return -1;
  }

  struct sockaddr_in server_address;
  struct sockaddr_in client_address;
  memset(&server_address, 0, sizeof(server_address));
  memset(&client_address, 0, sizeof(client_address));

  server_address.sin_addr.s_addr = htonl(INADDR_ANY);
  server_address.sin_port = htons(SERVER_PORT);
  server_address.sin_family = AF_INET;

  if (bind(sock_fd, (const struct sockaddr*)&server_address,
           sizeof(server_address)) < 0) {
    perror("Bind failed");
    close(sock_fd);
    return -1;
  }


  ret = run_handshake(&ctx, &sock_fd, &server_address, &client_address);
  close(sock_fd);
  edhoc_context_deinit(&ctx);
  return ret;
}
