/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 11/04/2026
 * @brief Testing with mocked libedhoc of the functions to add internal errors
 * to buffer
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#include <unity.h>

#include "edhoc/common/add_error/com_edhoc_add_internal_error.h"
#include "edhoc/common/add_error/tst_mock_edhoc_error_compose.h"

void setUp(void) { srv_edhoc_reset_error_compose_mock(); }

void test_add_internal_error_fails_on_invalid_buffer(void) {
  struct com_writable_buffer invalid_buffer = {.bytes = NULL, .capacity = 0};
  const char* description = "RANDOM DESCRIPTION";

  const enum com_edhoc_add_internal_error_to_response_status status =
      com_edhoc_add_internal_error_to_response(description, &invalid_buffer);

  TEST_ASSERT_EQUAL(COM_EDHOC_ADD_INTERNAL_ERROR_ERR_INVALID_RESPONSE_BUFFER,
                    status);
}

void test_add_internal_error_fails_on_error_compose_fail(void) {
  uint8_t error_buffer[5] = {0};
  const char* description = "RANDOM DESCRIPTION";
  struct com_writable_buffer response_buffer = {
      .bytes = error_buffer, .capacity = sizeof(error_buffer)};
  srv_edhoc_set_error_compose_failed();

  const enum com_edhoc_add_internal_error_to_response_status status =
      com_edhoc_add_internal_error_to_response(description, &response_buffer);

  TEST_ASSERT_EQUAL(COM_EDHOC_ADD_INTERNAL_ERROR_ERR_COMPOSE, status);
}
