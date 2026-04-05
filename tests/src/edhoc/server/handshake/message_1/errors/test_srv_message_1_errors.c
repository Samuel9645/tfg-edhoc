/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 05/04/2026
 * @brief Message 1 specific EDHOC error-response composition testing.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <unity.h>

#include "common/data_models.h"
#include "edhoc/server/handshake/message_1/srv_m1_errors.h"
#include "edhoc_context.h"

extern bool tst_stub_get_suites_called;
extern int32_t tst_stub_cipher_suites_to_return[8];
extern size_t tst_stub_cipher_suites_len_to_return;

static uint8_t resp_raw[128];
static struct com_writable_buffer resp_buf;
static struct edhoc_context dummy_ctx;

void setUp(void) {
  tst_stub_get_suites_called = false;
  tst_stub_cipher_suites_len_to_return = 0;
  memset(tst_stub_cipher_suites_to_return, 0,
         sizeof(tst_stub_cipher_suites_to_return));
  resp_buf = (struct com_writable_buffer){
      .bytes = resp_raw, .capacity = sizeof(resp_raw), .length = 0};
}

void test_add_error_generic_ignores_suites(void) {
  const int GENERIC_ERR = -100;

  edh_srv_message_1_handler_add_error(GENERIC_ERR, &dummy_ctx, "Fail",
                                      &resp_buf);

  TEST_ASSERT_FALSE_MESSAGE(
      tst_stub_get_suites_called,
      "Cipher suites should not be fetched for generic errors");
}

void test_add_error_suite_mismatch_uses_internal_struct(void) {
  const int32_t FIRST_BYTE = 0xAAAA;
  const int32_t SECOND_BYTE = 0xBBBB;
  tst_stub_cipher_suites_to_return[0] = FIRST_BYTE;
  tst_stub_cipher_suites_to_return[1] = SECOND_BYTE;
  tst_stub_cipher_suites_len_to_return = 2;

  edh_srv_message_1_handler_add_error(
      EDHOC_ERROR_CODE_WRONG_SELECTED_CIPHER_SUITE, &dummy_ctx, "Mismatch",
      &resp_buf);

  TEST_ASSERT_TRUE_MESSAGE(tst_stub_get_suites_called,
                           "Cipher suites must be fetched for mismatch errors");
  TEST_ASSERT_EQUAL_INT32(FIRST_BYTE, tst_stub_cipher_suites_to_return[0]);
  TEST_ASSERT_EQUAL_INT32(SECOND_BYTE, tst_stub_cipher_suites_to_return[1]);
}

void tearDown(void) {}