/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 31/03/2026
 * @brief Declarations of the variables used in the libedhoc stubs
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#include "edhoc/server/handshake/message_1/process/tst_srv_mock_m1_process_deps.h"

#include <edhoc.h>

static bool use_real_edhoc_context_init = true;
static int edhoc_context_init_result = EDHOC_SUCCESS;
static int message_1_process_result = EDHOC_SUCCESS;

void tst_srv_edhoc_m1_process_set_setup_failure(void) {
  use_real_edhoc_context_init = false;
  edhoc_context_init_result = EDHOC_ERROR_GENERIC_ERROR;
}

void tst_srv_edhoc_m1_process_set_process_failure(void) {
  use_real_edhoc_context_init = true;
  message_1_process_result = EDHOC_ERROR_GENERIC_ERROR;
}

void tst_srv_edhoc_m1_process_reset_mock_results(void) {
  use_real_edhoc_context_init = true;
  edhoc_context_init_result = EDHOC_SUCCESS;
  message_1_process_result = EDHOC_SUCCESS;
}

extern int __real_edhoc_context_init(  // NOLINT(*-reserved-identifier)
                                       // we need this
    struct edhoc_context* context, const struct edhoc_credentials* credentials);

int __wrap_edhoc_context_init(  // NOLINT(*-reserved-identifier)
                                // we need this
    struct edhoc_context* context,
    const struct edhoc_credentials* credentials) {
  if (use_real_edhoc_context_init) {
    return __real_edhoc_context_init(context, credentials);
  }
  return edhoc_context_init_result;
}

int edhoc_message_1_process(struct edhoc_context* edhoc_context,
                            const uint8_t* message_1,
                            const size_t message_1_length) {
  (void)edhoc_context;
  (void)message_1;
  (void)message_1_length;
  return message_1_process_result;
}
