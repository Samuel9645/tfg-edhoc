/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 01/04/2026
 * @brief Mock of the EDHOC context setup function of libedhoc for testing the
 * Message 1 process module.
 * @see [libedhoc
 * documentation](https://kamil-kielbasa.github.io/libedhoc/index.html)
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */
#include "edhoc/server/handshake/mocks/message_1/tst_srv_mock_edhoc_context_init.h"

#include <edhoc.h>
#include <stdbool.h>

static int edhoc_context_init_result = EDHOC_SUCCESS;
static bool use_real_edhoc_context_init = true;

void tst_srv_edhoc_m1_set_context_init_failure(void) {
  use_real_edhoc_context_init = false;
  edhoc_context_init_result = EDHOC_ERROR_INVALID_ARGUMENT;
}

void tst_srv_edhoc_m1_reset_context_init_mock(void) {
  use_real_edhoc_context_init = true;
  edhoc_context_init_result = EDHOC_SUCCESS;
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
