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

static bool use_real_extract_flow = true;

void tst_srv_edhoc_m1_set_extract_failed(void) {
  use_real_extract_flow = false;
}

void tst_srv_edhoc_m1_reset_extract_mock(void) { use_real_extract_flow = true; }

extern int __real_edhoc_extract_flow_info(  // NOLINT(*-reserved-identifier)
                                            // we need this
    struct edhoc_extracted_fields* extracted_fields);

int __wrap_edhoc_extract_flow_info(  // NOLINT(*-reserved-identifier)
                                     // we need this
    struct edhoc_extracted_fields* extracted_fields) {
  if (use_real_extract_flow) {
    return __real_edhoc_extract_flow_info(extracted_fields);
  }
  return EDHOC_ERROR_INVALID_ARGUMENT;
}