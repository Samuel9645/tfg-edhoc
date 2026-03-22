#include "coap/server_edhoc_dispatch.h"

#include <edhoc_helpers.h>

#include "coap/common/config.h"
#include "coap/shared/edhoc_request.h"
#include "coap/shared/edhoc_response.h"
#include "edhoc/common/config.h"
#include "edhoc/server/server_flow.h"

static struct edhoc_extracted_fields g_extracted_fields = {0};

void coap_server_dispatch_edhoc_post(coap_session_t* session,
                                     const coap_pdu_t* request,
                                     coap_pdu_t* response) {
  const uint8_t* request_payload = NULL;
  size_t request_len = 0;
  if (coap_shared_validate_edhoc_request(
          request, APPLICATION_CID_EDHOC_CBOR_SEQ, &request_payload,
          &request_len) != COAP_STATUS_SUCCESS) {
    coap_log_err("failed to validate EDHOC request\n");
    coap_pdu_set_code(response, COAP_RESPONSE_CODE_BAD_REQUEST);
    return;
  }

  if (coap_shared_add_edhoc_response_options(
          response, APPLICATION_EDHOC_CBOR_SEQ) != COAP_STATUS_SUCCESS) {
    coap_log_err("failed to add EDHOC response options\n");
    coap_pdu_set_code(response, COAP_RESPONSE_CODE_INTERNAL_ERROR);
    return;
  }

  uint8_t response_payload[MESSAGE_BUFFER_LENGTH] = {0};
  size_t response_len = 0;

  coap_response_data_t response_data = {
      .payload = response_payload,
      .payload_capacity = MESSAGE_BUFFER_LENGTH,
      .payload_len = &response_len,
  };

  coap_pdu_code_t response_code = COAP_RESPONSE_CODE_INTERNAL_ERROR;

  struct edhoc_context* edhoc_ctx =
      (struct edhoc_context*)coap_session_get_app_data(session);

  if (coap_shared_is_properly_formatted_message_1(request_payload,
                                                  request_len)) {
    server_edhoc_message_1_request_data_t request_data = {
        .session = session,
        .response = response,
        .request_data =
            {
                .payload = request_payload,
                .payload_len = request_len,
            },
    };
    response_code =
        server_edhoc_handle_message_1(&request_data, &response_data);
  } else if (edhoc_ctx != NULL && coap_shared_is_properly_formatted_message_3(
                                      request_payload, request_len, edhoc_ctx,
                                      &g_extracted_fields)) {
    server_edhoc_message_3_request_data_t request_data = {
        .session = session,
        .response = response,
        .request_data =
            {
                .payload = request_payload,
                .payload_len = request_len,
            },
        .extracted_fields = &g_extracted_fields,
    };
    response_code =
        server_edhoc_handle_message_3(&request_data, &response_data);
  } else {
    coap_log_err("received invalid or unexpected EDHOC message\n");
    coap_pdu_set_code(response, COAP_RESPONSE_CODE_INTERNAL_ERROR);
    return;
  }

  if (response_code != COAP_RESPONSE_CODE_CHANGED) {
    coap_log_err("EDHOC handler returned error code: %d\n", response_code);
    coap_pdu_set_code(response, response_code);
    return;
  }

  if (coap_shared_add_response_payload(response, response_payload,
                                       response_len) != COAP_STATUS_SUCCESS) {
    coap_log_err("failed to add response payload\n");
    coap_pdu_set_code(response, COAP_RESPONSE_CODE_INTERNAL_ERROR);
    return;
  }

  coap_pdu_set_code(response, COAP_RESPONSE_CODE_CHANGED);
}
