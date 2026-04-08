/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 08/04/2026
 * @brief Mocks of the error composition functions for testing error handling in
 * EDHOC server handshake
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#include "edhoc/common/tst_mock_add_edhoc_error_info.h"

#include <string.h>
#include <unity.h>

#include "edhoc/common/add_edhoc_error_info.h"
#include "edhoc/common/tst_response_buffer_helpers.h"

static const uint8_t MOCK_ERR_DATA[] = {0xFF};

void write_mock_buffer_to_response(struct com_writable_buffer* response_data) {
  memcpy(response_data->bytes, MOCK_ERR_DATA, sizeof(MOCK_ERR_DATA));
  response_data->length = sizeof(MOCK_ERR_DATA);
}

void tst_edh_srv_assert_handler_writes_error_in_buffer(
    const struct com_writable_buffer response) {
  TEST_ASSERT_EQUAL_MEMORY_MESSAGE(MOCK_ERR_DATA, response.bytes,
                                   sizeof(MOCK_ERR_DATA),
                                   "Error buffer was not written");
  TEST_ASSERT_EQUAL_MESSAGE(sizeof(MOCK_ERR_DATA), response.length,
                            "Error buffer length was not set correctly");
}

enum edh_com_add_internal_error_to_response_status
edh_com_add_internal_error_to_response(
    const char* error_message, struct com_writable_buffer* response_data) {
  (void)error_message;
  TEST_ASSERT_NOT_NULL_MESSAGE(error_message, "error message is NULL");

  write_mock_buffer_to_response(response_data);
  return EDH_COM_ADD_INTERNAL_ERROR_OK;
}

enum edh_com_add_edhoc_error_to_response_status
edh_com_add_edhoc_error_to_response_with_description(
    const struct edhoc_context* context, const char* error_description,
    struct com_writable_buffer* response_data) {
  (void)context;
  (void)error_description;
  TEST_ASSERT_NOT_NULL_MESSAGE(context, "context is NULL");
  edh_com_response_buffer_is_null(response_data);
  write_mock_buffer_to_response(response_data);
  return EDH_COM_ADD_ERROR_OK;
}