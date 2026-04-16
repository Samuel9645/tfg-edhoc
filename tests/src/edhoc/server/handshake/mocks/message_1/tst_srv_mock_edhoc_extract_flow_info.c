/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 08/04/2026
 * @brief Message 1 specific EDHOC parser mocks.
 * @see
 * [RFC 5.2.3](https://datatracker.ietf.org/doc/html/rfc9528/#name-responder-processing-of-mes)
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#include "edhoc/server/handshake/mocks/message_1/tst_srv_mock_edhoc_extract_flow_info.h"

#include <edhoc.h>
#include <edhoc_helpers.h>
#include <stdbool.h>

static bool extract_flow_should_fail = false;

void tst_srv_edhoc_m1_set_extract_failed(void) {
  extract_flow_should_fail = true;
}

void tst_srv_edhoc_m1_reset_parse_mock(void) {
  extract_flow_should_fail = false;
}

int edhoc_extract_flow_info(struct edhoc_extracted_fields* extracted_fields) {
  if (extract_flow_should_fail) {
    return EDHOC_ERROR_INVALID_ARGUMENT;
  }
  if (extracted_fields != NULL && extracted_fields->edhoc_message_ptr != NULL) {
    extracted_fields->edhoc_message_ptr += 1;
    extracted_fields->edhoc_message_size -= 1;
  }
  return EDHOC_SUCCESS;
}