/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 12/05/2026
 * @brief Mocked version of CoAP send function for testing
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

// ReSharper disable CppParameterMayBeConstPtrOrRef
#include "coap/client/mocks/tst_mock_exchange_coap_send_deps.h"

#include <edhoc_helpers.h>
#include <stdbool.h>
#include <string.h>

#include "coap/coap_config.h"

static uint8_t captured_payload[CONFIG_COAP_MAX_PDU_SIZE] = {0};
static size_t last_request_length = 0;
static enum config_coap_content_format_edhoc_values last_config_format =
    CONFIG_COAP_CONTENT_EDHOC;

enum status_coap cli_coap_mock_send(
    struct cli_coap_exchange* exchange,
    const struct cli_coap_exchange_request request_data) {
  (void)exchange;
  if (com_readonly_buffer_has_content(request_data.buffer)) {
    memcpy(captured_payload, request_data.buffer.bytes,
           request_data.buffer.length);
    last_request_length = request_data.buffer.length;
    last_config_format = request_data.content_format;
  }
  return STATUS_COAP_OK;
}

struct cli_coap_exchange_request tst_mock_coap_get_last_sent_data(void) {
  return (struct cli_coap_exchange_request){
      .buffer = {.bytes = captured_payload, .length = last_request_length},
      .content_format = last_config_format};
}

static int prepend_flow_result = EDHOC_SUCCESS;
static bool use_real_prepend_flow = true;

void tst_cli_prepend_flow_set_failure(void) {
  prepend_flow_result = EDHOC_ERROR_BUFFER_TOO_SMALL;
  use_real_prepend_flow = false;
}

void tst_cli_prepend_flow_reset_mock(void) {
  prepend_flow_result = EDHOC_SUCCESS;
  use_real_prepend_flow = true;
}

extern int __real_edhoc_prepend_flow(  // NOLINT(*-reserved-identifier)
    struct edhoc_prepended_fields* fields);

int __wrap_edhoc_prepend_flow(  // NOLINT(*-reserved-identifier)
    struct edhoc_prepended_fields* fields) {
  if (use_real_prepend_flow) {
    return __real_edhoc_prepend_flow(fields);
  }
  return prepend_flow_result;
}

static int recalculate_size_result = EDHOC_SUCCESS;
static bool use_real_recalculate_size = true;

void tst_cli_recalculate_size_set_failure(void) {
  recalculate_size_result = EDHOC_ERROR_BUFFER_TOO_SMALL;
  use_real_recalculate_size = false;
}

void tst_cli_recalculate_size_reset_mock(void) {
  recalculate_size_result = EDHOC_SUCCESS;
  use_real_recalculate_size = true;
}

extern int
__real_edhoc_prepend_recalculate_size(  // NOLINT(*-reserved-identifier)
    struct edhoc_prepended_fields* fields);

int __wrap_edhoc_prepend_recalculate_size(  // NOLINT(*-reserved-identifier)
    struct edhoc_prepended_fields* fields) {
  if (use_real_recalculate_size) {
    return __real_edhoc_prepend_recalculate_size(fields);
  }
  return recalculate_size_result;
}

static int prepend_connection_id_result = EDHOC_SUCCESS;
static bool use_real_prepend_connection_id = true;

void tst_cli_prepend_connection_id_set_failure(void) {
  prepend_connection_id_result = EDHOC_ERROR_INVALID_ARGUMENT;
  use_real_prepend_connection_id = false;
}

void tst_cli_prepend_connection_id_reset_mock(void) {
  prepend_connection_id_result = EDHOC_SUCCESS;
  use_real_prepend_connection_id = true;
}

extern int __real_edhoc_prepend_connection_id(  // NOLINT(*-reserved-identifier)
    struct edhoc_prepended_fields* fields,
    const struct edhoc_connection_id* cid);

int __wrap_edhoc_prepend_connection_id(  // NOLINT(*-reserved-identifier)
    struct edhoc_prepended_fields* fields,
    const struct edhoc_connection_id* cid) {
  if (use_real_prepend_connection_id) {
    return __real_edhoc_prepend_connection_id(fields, cid);
  }
  return prepend_connection_id_result;
}

void tst_cli_prepend_reset_all_mocks(void) {
  tst_cli_prepend_connection_id_reset_mock();
  tst_cli_prepend_flow_reset_mock();
  tst_cli_recalculate_size_reset_mock();
}
