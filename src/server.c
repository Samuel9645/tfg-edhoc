#include "server.h"

#include <coap3/coap.h>
#include <coap3/coap_session.h>
#include <edhoc_helpers.h>
#include <edhoc_values.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "coap/common/config.h"
#include "coap/common/helpers.h"
#include "coap/server_utils.h"
#include "common/cleanup.h"
#include "edhoc/common/config.h"
#include "edhoc/common/setup.h"
#include "edhoc/credentials/authentication.h"
#include "edhoc/credentials/public_data.h"
#include "edhoc/credentials/server_private_key.h"

// TODO: Check for reverse flow support

static int server_credential_fetch(void* user_context,
                                   struct edhoc_auth_creds* credentials) {
  return credential_fetch(user_context, credentials, SERVER_PUBLIC_KEY,
                          sizeof(SERVER_PUBLIC_KEY), SERVER_PRIVATE_KEY,
                          sizeof(SERVER_PRIVATE_KEY), SERVER_KID);
}

static int server_credential_verify(void* user_context,
                                    struct edhoc_auth_creds* credentials,
                                    const uint8_t** public_key_reference,
                                    size_t* public_key_length) {
  return credential_verify(user_context, credentials, CLIENT_KID,
                           CLIENT_PUBLIC_KEY, ARRAY_SIZE(CLIENT_PUBLIC_KEY),
                           public_key_reference, public_key_length);
}

static const struct edhoc_credentials credentials = {
    .fetch = server_credential_fetch,
    .verify = server_credential_verify,
};

static void edhoc_post_handler(coap_resource_t* resource,
                               coap_session_t* session,
                               const coap_pdu_t* request,
                               const coap_string_t* query,
                               coap_pdu_t* response) {
  (void)resource;
  (void)session;
  (void)query;
  coap_opt_iterator_t opt_iter = {0};
  coap_opt_t* option =
      coap_check_option(request, COAP_OPTION_CONTENT_FORMAT, &opt_iter);
  if (!option) {
    coap_log_err("missing content format option\n");
    coap_pdu_set_code(response, COAP_RESPONSE_CODE_BAD_REQUEST);
    return;
  }
  uint16_t content_format =
      coap_decode_var_bytes(coap_opt_value(option), coap_opt_length(option));
  if (content_format != APPLICATION_CID_EDHOC_CBOR_SEQ) {
    coap_log_err("invalid content format\n");
    coap_pdu_set_code(response, COAP_RESPONSE_CODE_BAD_REQUEST);
    return;
  }
  size_t size = 0;
  const uint8_t* request_pdu_data = NULL;
  if (!coap_get_data(request, &size, &request_pdu_data)) {
    coap_log_err("cannot get request request pdu data\n");
    coap_pdu_set_code(response, COAP_RESPONSE_CODE_BAD_REQUEST);
    return;
  }
  // TODO: EXTRACT THIS INTO FUNCTION
  const bool is_message_1 = (size > 0 && request_pdu_data[0] == CBOR_TRUE);
  struct edhoc_context* edhoc_ctx =
      (struct edhoc_context*)coap_session_get_app_data(session);

  coap_optlist_t* optlist =
      create_coap_edhoc_optlist(APPLICATION_EDHOC_CBOR_SEQ);
  if (!optlist) {
    coap_log_err("cannot create options list\n");
    coap_pdu_set_code(response, COAP_RESPONSE_CODE_INTERNAL_ERROR);
    return;
  }
  if (!coap_add_optlist_pdu(response, &optlist)) {
    coap_log_err("cannot add options to response\n");
    coap_pdu_set_code(response, COAP_RESPONSE_CODE_INTERNAL_ERROR);
    return;
  }

  if (is_message_1) {
    if (edhoc_ctx != NULL) {
      coap_log_err("EDHOC context already exists for this session\n");
      coap_pdu_set_code(response, COAP_RESPONSE_CODE_INTERNAL_ERROR);
      return;
    }
    const uint8_t* edhoc_msg1_bytes = request_pdu_data + 1;
    size_t edhoc_msg1_len = size - 1;

    edhoc_ctx = malloc(sizeof(struct edhoc_context));
    if (!edhoc_ctx) {
      coap_log_err("cannot allocate memory for EDHOC context\n");
      coap_pdu_set_code(response, COAP_RESPONSE_CODE_INTERNAL_ERROR);
      return;
    }

    if (edhoc_setup_context(edhoc_ctx, &credentials) != EDHOC_SUCCESS) {
      coap_log_err("cannot setup EDHOC context\n");
      free(edhoc_ctx);
      coap_pdu_set_code(response, COAP_RESPONSE_CODE_INTERNAL_ERROR);
      return;
    }
    if (coap_session_set_app_data2(session, edhoc_ctx, free) != NULL) {
      coap_log_err("app data for session already set\n");
      free(edhoc_ctx);
      coap_pdu_set_code(response, COAP_RESPONSE_CODE_INTERNAL_ERROR);
      return;
    }
    if (edhoc_message_1_process(edhoc_ctx, edhoc_msg1_bytes, edhoc_msg1_len) !=
        EDHOC_SUCCESS) {
      // TODO: Check if the error from the EDHOC context can be used to set a
      // more specific CoAP response code
      // https://kamil-kielbasa.github.io/libedhoc/api.html#_CPPv427edhoc_message_error_processPK7uint8_t6size_tP16edhoc_error_codeP16edhoc_error_info
      enum edhoc_error_code err;
      if (edhoc_error_get_code(edhoc_ctx, &err) != EDHOC_SUCCESS) {
        coap_log_err("cannot get error code from EDHOC context\n");
        coap_pdu_set_code(response, COAP_RESPONSE_CODE_INTERNAL_ERROR);
        return;
      }
      coap_log_err("cannot process Message 1, error code: %d\n", err);
      coap_pdu_set_code(response, COAP_RESPONSE_CODE_BAD_REQUEST);
      return;
    }

    size_t message2_len = 0;
    uint8_t message2_buffer[MESSAGE_BUFFER_LENGTH] = {0};
    if (edhoc_message_2_compose(edhoc_ctx, message2_buffer,
                                MESSAGE_BUFFER_LENGTH,
                                &message2_len) != EDHOC_SUCCESS) {
      enum edhoc_error_code err;
      if (edhoc_error_get_code(edhoc_ctx, &err) != EDHOC_SUCCESS) {
        coap_log_err("cannot get error code from EDHOC context\n");
        coap_pdu_set_code(response, COAP_RESPONSE_CODE_INTERNAL_ERROR);
        return;
      }
      coap_log_err("cannot compose Message 2, error code: %d\n", err);
      coap_pdu_set_code(response, COAP_RESPONSE_CODE_INTERNAL_ERROR);
      return;
    }
    if (!coap_add_data(response, message2_len, message2_buffer)) {
      coap_log_err("cannot add data to response\n");
      coap_pdu_set_code(response, COAP_RESPONSE_CODE_INTERNAL_ERROR);
      return;
    }
    coap_pdu_set_code(response, COAP_RESPONSE_CODE_CHANGED);

  } else {
    if (!edhoc_ctx) {
      coap_log_err("no EDHOC context for this session\n");
      coap_pdu_set_code(response, COAP_RESPONSE_CODE_BAD_REQUEST);
      return;
    }

    struct edhoc_extracted_fields extracted_fields = {
        .buffer = request_pdu_data,
        .buffer_size = size,
        .edhoc_message_ptr = request_pdu_data,
        .edhoc_message_size = size};
    if (edhoc_extract_connection_id(&extracted_fields) != EDHOC_SUCCESS) {
      coap_pdu_set_code(response, COAP_RESPONSE_CODE_BAD_REQUEST);
      return;
    }
    if (!edhoc_connection_id_equal(&extracted_fields.extracted_conn_id,
                                   &edhoc_ctx->private_cid)) {
      coap_pdu_set_code(response, COAP_RESPONSE_CODE_BAD_REQUEST);
      return;
    }

    if (edhoc_message_3_process(edhoc_ctx, extracted_fields.edhoc_message_ptr,
                                extracted_fields.edhoc_message_size) !=
        EDHOC_SUCCESS) {
      enum edhoc_error_code err;
      if (edhoc_error_get_code(edhoc_ctx, &err) != EDHOC_SUCCESS) {
        coap_log_err("cannot get error code from EDHOC context\n");
        coap_pdu_set_code(response, COAP_RESPONSE_CODE_INTERNAL_ERROR);
        return;
      }
      coap_log_err("cannot process Message 3, error code: %d\n", err);
      coap_pdu_set_code(response, COAP_RESPONSE_CODE_BAD_REQUEST);
      return;
    }
    size_t message4_len = 0;
    uint8_t message4_buffer[MESSAGE_BUFFER_LENGTH] = {0};

    if (edhoc_message_4_compose(edhoc_ctx, message4_buffer,
                                MESSAGE_BUFFER_LENGTH,
                                &message4_len) != EDHOC_SUCCESS) {
      coap_log_err("cannot compose Message 4\n");
      coap_pdu_set_code(response, COAP_RESPONSE_CODE_INTERNAL_ERROR);
      return;
    }
    if (!coap_add_data(response, message4_len, message4_buffer)) {
      coap_log_err("cannot add data to response\n");
      coap_pdu_set_code(response, COAP_RESPONSE_CODE_INTERNAL_ERROR);
      return;
    }
    coap_pdu_set_code(response, COAP_RESPONSE_CODE_CHANGED);
  }
}

emulation_status_t run_server(void) {
  session_resources_t server_resources = {0};

  server_resources.coap_context = coap_server_create_context();
  if (!server_resources.coap_context) {
    cleanup_resources(&server_resources);
    return EMULATION_FAILURE;
  }

  static const char COAP_LISTEN_UCAST_IP[] = "::";
  coap_status_result_t result = coap_server_setup_endpoints(
      server_resources.coap_context, COAP_LISTEN_UCAST_IP);
  if (result != COAP_STATUS_SUCCESS) {
    cleanup_resources(&server_resources);
    return EMULATION_FAILURE;
  }

  result = coap_server_add_post_resource(
      server_resources.coap_context, ".well-known/edhoc", edhoc_post_handler);
  if (result != COAP_STATUS_SUCCESS) {
    cleanup_resources(&server_resources);
    return EMULATION_FAILURE;
  }

  // coap_session_set_app_data2(server_resources.coap_session,
  //                            &edhoc_session_context, NULL);

  result = coap_server_run_input_output_loop(server_resources.coap_context);
  if (result != COAP_STATUS_SUCCESS) {
    cleanup_resources(&server_resources);
    return EMULATION_FAILURE;
  }

  cleanup_resources(&server_resources);
  return EMULATION_SUCCESS;
}

// static int run_handshake(struct edhoc_context* context, int* socket_fd,
//                          struct sockaddr_in* server_address,
//                          struct sockaddr_in* client_address) {
//   socklen_t client_address_len = sizeof(*client_address);

//   printf("Server listening on port %d...\n",
//   ntohs(server_address->sin_port));

//   uint8_t message_buffer[MESSAGE_BUFFER_LENGTH] = {0};
//   const size_t message_buffer_size = sizeof(message_buffer);

//   ssize_t message1_length =
//       recvfrom(*socket_fd, message_buffer, message_buffer_size, 0,
//                (struct sockaddr*)client_address, &client_address_len);
//   printf("Received Message 1 (%zd bytes)\n", message1_length);

//   if (edhoc_message_1_process(context, message_buffer, message1_length) !=
//       EDHOC_SUCCESS) {
//     fprintf(stderr, "cannot process Message 1\n");
//     return -1;
//   }

//   size_t message2_length = 0;
//   if (edhoc_message_2_compose(context, message_buffer, message_buffer_size,
//                               &message2_length) != EDHOC_SUCCESS) {
//     fprintf(stderr, "cannot compose Message 2\n");
//     return -1;
//   }

//   sendto(*socket_fd, message_buffer, message2_length, 0,
//          (const struct sockaddr*)client_address, client_address_len);
//   printf("Sent Message 2 (%zu bytes)\n", message2_length);

//   ssize_t message3_length =
//       recvfrom(*socket_fd, message_buffer, message_buffer_size, 0,
//                (struct sockaddr*)client_address, &client_address_len);
//   printf("Received Message 3 (%zd bytes)\n", message3_length);

//   if (edhoc_message_3_process(context, message_buffer, message3_length) !=
//       EDHOC_SUCCESS) {
//     fprintf(stderr, "cannot process Message 3\n");
//     return -1;
//   }

//   printf("Server: EDHOC Handshake Completed Successfully!\n");

//   return EDHOC_SUCCESS;
// }

// /**
//  * @brief Receive and decrypt a message from the client after handshake
//  * @param context EDHOC context for exporting shared secret
//  * @param socket_fd UDP socket file descriptor
//  * @param client_address Client address structure
//  * @param received_message_buffer_size Size of output buffer
//  * @param[out] received_message_buffer Output buffer for decrypted message
//  * @param[out] received_message_length Actual length of decrypted message
//  * @return Status code
//  */
// static int receive_message(struct edhoc_context* context, int socket_fd,
//                            struct sockaddr_in* client_address,
//                            size_t received_message_buffer_size,
//                            uint8_t* received_message_buffer,
//                            size_t* received_message_length) {
//   uint8_t shared_secret[EXPORTED_SECRET_LENGTH] = {0};
//   int result = edhoc_export_prk_exporter(context, PKR_OUT_LABEL,
//   shared_secret,
//                                          sizeof(shared_secret));
//   if (result != EDHOC_SUCCESS) {
//     return result;
//   }

//   uint8_t encrypted_message[CIPHERTEXT_MAX_LENGTH] = {0};
//   socklen_t client_address_len = sizeof(*client_address);
//   result = recvfrom(socket_fd, encrypted_message,
//   sizeof(encrypted_message), 0,
//                     (struct sockaddr*)client_address, &client_address_len);
//   if (result < 0) {
//     fprintf(stderr, "cannot receive ciphertext\n");
//     return -1;
//   }
//   if (result == 0) {
//     fprintf(stderr, "Received empty message\n");
//     return -1;
//   }

//   size_t encrypted_message_length = (size_t)result;
//   const psa_status_t decrypt_status = shared_decrypt_ciphertext(
//       encrypted_message, encrypted_message_length, shared_secret,
//       sizeof(shared_secret), received_message_buffer,
//       received_message_buffer_size, received_message_length);
//   if (decrypt_status != PSA_SUCCESS) {
//     fprintf(stderr, "cannot decrypt ciphertext\n");
//   }
//   return EDHOC_SUCCESS;
// }