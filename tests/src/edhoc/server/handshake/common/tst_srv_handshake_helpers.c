/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 30/03/2026
 * @brief Constants used across EDHOC server handshake tests.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */
#include <stdbool.h>
#include <string.h>

#include "edhoc/server/handshake/common/tst_srv_m1_helpers.h"
#include "unity.h"

const uint8_t TST_CANARY_PATTERN = 0xAA;
const uint8_t TST_NONZERO_VALUE = 0xFF;

static void assert_length_is_untouched(const com_writable_buffer_t* response) {
  TEST_ASSERT_EQUAL_UINT32(TST_NONZERO_VALUE, response->length);
}

static void assert_buffer_is_untouched(const com_writable_buffer_t* response) {
  TEST_ASSERT_EACH_EQUAL_UINT8_MESSAGE(
      TST_CANARY_PATTERN, response->bytes, response->capacity,
      "Handler modified the response buffer when it should have failed early.");
}

static bool buffer_provided(const com_writable_buffer_t* response) {
  return response->bytes != NULL;
}

void tst_edh_srv_assert_response_clean(const com_writable_buffer_t* response) {
  TEST_ASSERT_NOT_NULL_MESSAGE(response, "Response struct pointer is NULL");
  if (buffer_provided(response)) {
    assert_length_is_untouched(response);
    assert_buffer_is_untouched(response);
  } else {
    assert_length_is_untouched(response);
  }
}

com_writable_buffer_t tst_edh_invalid_response(void) {
  return (com_writable_buffer_t){
      .bytes = NULL, .capacity = 0, .length = TST_NONZERO_VALUE};
}

void tst_edh_reset_response(com_writable_buffer_t* response) {
  TEST_ASSERT_NOT_NULL(response);
  TEST_ASSERT_NOT_NULL(response->bytes);

  response->length = TST_NONZERO_VALUE;
  memset(response->bytes, TST_CANARY_PATTERN, response->capacity);
}
