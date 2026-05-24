/**
 * @file
 * @brief Linker-wrap helpers for dispatcher tests.
 */

#include "coap/server/dispatch/mocks/srv_mock_dispatch_deps.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

#include "edhoc/common/com_edhoc_setup_context.h"

static bool use_real_calloc = true;
static bool use_real_setup_context = true;

void tst_srv_set_calloc_failure(void) { use_real_calloc = false; }

void tst_srv_set_calloc_ok(void) { use_real_calloc = false; }

void tst_srv_set_setup_context_failure(void) { use_real_setup_context = false; }

void tst_srv_dispatch_reset_mock(void) {
  use_real_calloc = true;
  use_real_setup_context = true;
}

extern void* __real_calloc(size_t nmemb, size_t size);  // NOLINT

void* __wrap_calloc(size_t nmemb, size_t size) {  // NOLINT
  if (use_real_calloc) {
    return __real_calloc(nmemb, size);
  }
  return NULL;
}

extern struct com_edhoc_setup_context_result
__real_com_edhoc_setup_context(  // NOLINT
    struct edhoc_context* context,
    struct com_edhoc_parameters edhoc_parameters);  // NOLINT

struct com_edhoc_setup_context_result __wrap_com_edhoc_setup_context(  // NOLINT
    struct edhoc_context* context,
    struct com_edhoc_parameters edhoc_parameters) {  // NOLINT
  if (use_real_setup_context) {
    return __real_com_edhoc_setup_context(context, edhoc_parameters);
  }
  return (struct com_edhoc_setup_context_result){
      .status = COM_EDHOC_SETUP_CTX_ERR_INVALID_EDHOC_PARAMETERS,
  };
}
