/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 29/04/2026
 * @brief Mock of the EDHOC setup context function and its dependencies for
 * testing the setup context module.
 * @see [libedhoc
 * documentation](https://kamil-kielbasa.github.io/libedhoc/index.html)
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

// ReSharper disable CppParameterMayBeConstPtrOrRef
// ReSharper disable CppParameterMayBeConst
#include "edhoc/common/setup_context/mocks/tst_com_mock_edhoc_setup_context.h"

#include <edhoc.h>
#include <psa/crypto.h>
#include <stdbool.h>

static bool use_real_psa_crypto_init = true;
static psa_status_t psa_crypto_init_result = PSA_SUCCESS;

extern psa_status_t __real_psa_crypto_init(  // NOLINT(*-reserved-identifier)
    void);

psa_status_t __wrap_psa_crypto_init(void) {  // NOLINT(*-reserved-identifier)
  if (use_real_psa_crypto_init) {
    return __real_psa_crypto_init();
  }
  return psa_crypto_init_result;
}

void tst_com_set_psa_crypto_init_failure(void) {
  use_real_psa_crypto_init = false;
  psa_crypto_init_result = PSA_ERROR_GENERIC_ERROR;
}

void tst_com_use_real_psa_crypto_init(void) {
  use_real_psa_crypto_init = true;
  psa_crypto_init_result = PSA_SUCCESS;
}

static bool use_real_edhoc_context_init = true;
static int edhoc_context_init_result = EDHOC_SUCCESS;

extern int __real_edhoc_context_init(  // NOLINT(*-reserved-identifier)
    struct edhoc_context* context);

int __wrap_edhoc_context_init(  // NOLINT(*-reserved-identifier)
    struct edhoc_context* context) {
  if (use_real_edhoc_context_init) {
    return __real_edhoc_context_init(context);
  }
  return edhoc_context_init_result;
}

void tst_com_set_edhoc_context_init_failure(void) {
  use_real_edhoc_context_init = false;
  edhoc_context_init_result = EDHOC_ERROR_INVALID_ARGUMENT;
}

void tst_com_use_real_edhoc_context_init(void) {
  use_real_edhoc_context_init = true;
  edhoc_context_init_result = EDHOC_SUCCESS;
}

static bool use_real_edhoc_set_methods = true;
static int edhoc_set_methods_result = EDHOC_SUCCESS;

extern int __real_edhoc_set_methods(  // NOLINT(*-reserved-identifier)
    struct edhoc_context* context, const enum edhoc_method* methods,
    size_t methods_len);

int __wrap_edhoc_set_methods(  // NOLINT(*-reserved-identifier)
    struct edhoc_context* context, const enum edhoc_method* methods,
    size_t methods_len) {
  if (use_real_edhoc_set_methods) {
    return __real_edhoc_set_methods(context, methods, methods_len);
  }
  return edhoc_set_methods_result;
}

void tst_com_set_edhoc_set_methods_failure(void) {
  use_real_edhoc_set_methods = false;
  edhoc_set_methods_result = EDHOC_ERROR_INVALID_ARGUMENT;
}

void tst_com_use_real_edhoc_set_methods(void) {
  use_real_edhoc_set_methods = true;
  edhoc_set_methods_result = EDHOC_SUCCESS;
}

static bool use_real_edhoc_set_cipher_suites = true;
static int edhoc_set_cipher_suites_result = EDHOC_SUCCESS;

extern int __real_edhoc_set_cipher_suites(  // NOLINT(*-reserved-identifier)
    struct edhoc_context* context, const struct edhoc_cipher_suite* suites,
    size_t suites_len);

int __wrap_edhoc_set_cipher_suites(  // NOLINT(*-reserved-identifier)
    struct edhoc_context* context, const struct edhoc_cipher_suite* suites,
    size_t suites_len) {
  if (use_real_edhoc_set_cipher_suites) {
    return __real_edhoc_set_cipher_suites(context, suites, suites_len);
  }
  return edhoc_set_cipher_suites_result;
}

void tst_com_set_edhoc_set_cipher_suites_failure(void) {
  use_real_edhoc_set_cipher_suites = false;
  edhoc_set_cipher_suites_result = EDHOC_ERROR_INVALID_ARGUMENT;
}

void tst_com_use_real_edhoc_set_cipher_suites(void) {
  use_real_edhoc_set_cipher_suites = true;
  edhoc_set_cipher_suites_result = EDHOC_SUCCESS;
}

static bool use_real_edhoc_set_connection_id = true;
static int edhoc_set_connection_id_result = EDHOC_SUCCESS;

extern int __real_edhoc_set_connection_id(  // NOLINT(*-reserved-identifier)
    struct edhoc_context* context, const struct edhoc_connection_id* cid);

int __wrap_edhoc_set_connection_id(  // NOLINT(*-reserved-identifier)
    struct edhoc_context* context, const struct edhoc_connection_id* cid) {
  if (use_real_edhoc_set_connection_id) {
    return __real_edhoc_set_connection_id(context, cid);
  }
  return edhoc_set_connection_id_result;
}

void tst_com_set_edhoc_set_connection_id_failure(void) {
  use_real_edhoc_set_connection_id = false;
  edhoc_set_connection_id_result = EDHOC_ERROR_INVALID_ARGUMENT;
}

void tst_com_use_real_edhoc_set_connection_id(void) {
  use_real_edhoc_set_connection_id = true;
  edhoc_set_connection_id_result = EDHOC_SUCCESS;
}

static bool use_real_edhoc_bind_keys = true;
static int edhoc_bind_keys_result = EDHOC_SUCCESS;

extern int __real_edhoc_bind_keys(  // NOLINT(*-reserved-identifier)
    struct edhoc_context* context, const struct edhoc_keys* keys);

int __wrap_edhoc_bind_keys(  // NOLINT(*-reserved-identifier)
    struct edhoc_context* context, const struct edhoc_keys* keys) {
  if (use_real_edhoc_bind_keys) {
    return __real_edhoc_bind_keys(context, keys);
  }
  return edhoc_bind_keys_result;
}

void tst_com_set_edhoc_bind_keys_failure(void) {
  use_real_edhoc_bind_keys = false;
  edhoc_bind_keys_result = EDHOC_ERROR_INVALID_ARGUMENT;
}

void tst_com_use_real_edhoc_bind_keys(void) {
  use_real_edhoc_bind_keys = true;
  edhoc_bind_keys_result = EDHOC_SUCCESS;
}

static bool use_real_edhoc_bind_crypto = true;
static int edhoc_bind_crypto_result = EDHOC_SUCCESS;

extern int __real_edhoc_bind_crypto(  // NOLINT(*-reserved-identifier)
    struct edhoc_context* context, const struct edhoc_crypto* crypto);

int __wrap_edhoc_bind_crypto(  // NOLINT(*-reserved-identifier)
    struct edhoc_context* context, const struct edhoc_crypto* crypto) {
  if (use_real_edhoc_bind_crypto) {
    return __real_edhoc_bind_crypto(context, crypto);
  }
  return edhoc_bind_crypto_result;
}

void tst_com_set_edhoc_bind_crypto_failure(void) {
  use_real_edhoc_bind_crypto = false;
  edhoc_bind_crypto_result = EDHOC_ERROR_INVALID_ARGUMENT;
}

void tst_com_use_real_edhoc_bind_crypto(void) {
  use_real_edhoc_bind_crypto = true;
  edhoc_bind_crypto_result = EDHOC_SUCCESS;
}

static bool use_real_edhoc_set_user_context = true;
static int edhoc_set_user_context_result = EDHOC_SUCCESS;

extern int __real_edhoc_set_user_context(  // NOLINT(*-reserved-identifier)
    struct edhoc_context* edhoc_context, void* user_context);

int __wrap_edhoc_set_user_context(  // NOLINT(*-reserved-identifier)
    struct edhoc_context* edhoc_context, void* user_context) {
  if (use_real_edhoc_set_user_context) {
    return __real_edhoc_set_user_context(edhoc_context, user_context);
  }
  return edhoc_set_user_context_result;
}

void tst_com_set_edhoc_set_user_context_failure(void) {
  use_real_edhoc_set_user_context = false;
  edhoc_set_user_context_result = EDHOC_ERROR_INVALID_ARGUMENT;
}

void tst_com_use_real_edhoc_set_user_context(void) {
  use_real_edhoc_set_user_context = true;
  edhoc_bind_crypto_result = EDHOC_SUCCESS;
}

static bool use_real_edhoc_bind_credentials = true;
static int edhoc_bind_credentials_result = EDHOC_SUCCESS;

extern int __real_edhoc_bind_credentials(  // NOLINT(*-reserved-identifier)
    struct edhoc_context* context, const struct edhoc_credentials* creds);

int __wrap_edhoc_bind_credentials(  // NOLINT(*-reserved-identifier)
    struct edhoc_context* context, const struct edhoc_credentials* creds) {
  if (use_real_edhoc_bind_credentials) {
    return __real_edhoc_bind_credentials(context, creds);
  }
  return edhoc_bind_credentials_result;
}

void tst_com_set_edhoc_bind_credentials_failure(void) {
  use_real_edhoc_bind_credentials = false;
  edhoc_bind_credentials_result = EDHOC_ERROR_INVALID_ARGUMENT;
}

void tst_com_use_real_edhoc_bind_credentials(void) {
  use_real_edhoc_bind_credentials = true;
  edhoc_bind_credentials_result = EDHOC_SUCCESS;
}

void tst_com_reset_setup_context_mock(void) {
  tst_com_use_real_psa_crypto_init();
  tst_com_use_real_edhoc_context_init();
  tst_com_use_real_edhoc_set_methods();
  tst_com_use_real_edhoc_set_cipher_suites();
  tst_com_use_real_edhoc_set_connection_id();
  tst_com_use_real_edhoc_bind_keys();
  tst_com_use_real_edhoc_bind_crypto();
  tst_com_use_real_edhoc_set_user_context();
  tst_com_use_real_edhoc_bind_credentials();
}
