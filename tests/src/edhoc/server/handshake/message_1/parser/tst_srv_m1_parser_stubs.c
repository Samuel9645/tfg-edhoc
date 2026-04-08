/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 08/04/2026
 * @brief Message 1 specific EDHOC parser stubs.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#include "edhoc/server/handshake/message_1/parser/tst_srv_m1_parser_stubs.h"

#include <edhoc.h>
#include <edhoc_helpers.h>
#include <stdbool.h>

static bool extract_flow_should_fail = false;

void tst_stub_extract_flow_set_failed(void) { extract_flow_should_fail = true; }

void srv_m1_parser_reset_stubs(void) { extract_flow_should_fail = false; }

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