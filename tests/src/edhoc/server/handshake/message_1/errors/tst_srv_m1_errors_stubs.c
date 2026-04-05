/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 5/04/2026
 * @brief Stubs to link to when testing the error handlers in Message 1
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */
// ReSharper disable CppParameterMayBeConstPtrOrRef
// ReSharper disable CppParameterMayBeConst
// since the real implementations expects non-const pointers to allow output
// parameters
// ReSharper disable CppParameterNamesMismatch
#include <edhoc.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

/* Mock State - The test file will modify these */
bool tst_stub_get_suites_called = false;
int32_t tst_stub_cipher_suites_to_return[8] = {0};
size_t tst_stub_cipher_suites_len_to_return = 0;

/* Stubbed version of libedhoc function */
int edhoc_error_get_cipher_suites(
    const struct edhoc_context *ctx, int32_t *csuites, size_t csuites_size,
    size_t *csuites_len,
    int32_t *peer_csuites,  // NOLINT(*-non-const-parameter)
    size_t peer_csuites_size,
    size_t *peer_csuites_len) {  // NOLINT(*-non-const-parameter)
  (void)ctx;
  (void)csuites;
  (void)csuites_size;
  (void)csuites_len;
  (void)peer_csuites;
  (void)peer_csuites_len;
  (void)peer_csuites_size;
  tst_stub_get_suites_called = true;
  if (csuites && csuites_len && tst_stub_cipher_suites_len_to_return > 0) {
    const size_t count = tst_stub_cipher_suites_len_to_return < csuites_size
                             ? tst_stub_cipher_suites_len_to_return
                             : csuites_size;

    memcpy(csuites, tst_stub_cipher_suites_to_return, count * sizeof(int32_t));
    *csuites_len = count;
  }
  return 0;  // EDHOC_SUCCESS
}