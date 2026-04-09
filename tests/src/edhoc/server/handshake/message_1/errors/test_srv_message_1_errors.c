/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 05/04/2026
 * @brief Message 1 specific EDHOC error-response composition testing.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */
#include <stdint.h>
#include <string.h>
#include <unity.h>

#include "common/com_data_models.h"
#include "edhoc/server/handshake/message_1/errors/tst_srv_m1_errors_stubs.h"
#include "edhoc/server/handshake/message_1/srv_m1_errors.h"
#include "edhoc_context.h"

// static struct test_server_message_1_error_env {
//   uint8_t response_raw_buffer[128];
//   struct com_writable_buffer response_buffer;
//   struct edhoc_context dummy_context;
// } env = {0};
//
// static void test_server_message_1_error_setup_env(
//     struct test_server_message_1_error_env* env_pointer) {
//   memset(env_pointer, 0, sizeof(*env_pointer));
//   env_pointer->response_buffer = (struct com_writable_buffer){
//       .bytes = env_pointer->response_raw_buffer,
//       .capacity = sizeof(env_pointer->response_raw_buffer),
//       .length = 0};
// }
//
// void setUp(void) {
//   tst_srv_m1_err_reset_stub_results();
//   test_server_message_1_error_setup_env(&env);
// }
//
// void test_add_error_ignores_cipher_suites_with_generic_errors(void) {
//   const enum edh_srv_message_1_handler_add_error_status status =
//       edh_srv_message_1_handler_add_error(
//           EDHOC_ERROR_GENERIC_ERROR, &env.dummy_context,
//           "Generic Failure Message", &env.response_buffer);
//
//   TEST_ASSERT_EQUAL_MESSAGE(EDH_SRV_MSG1_ADD_ERROR_OK, status,
//                             "The error reporter should return a successful "
//                             "status for generic errors");
//
//   TEST_ASSERT_FALSE_MESSAGE(tst_stub_get_suites_called,
//                             "The cipher suites should NOT be fetched from the
//                             " "context for generic errors");
// }
//
// void test_add_error_writes_cipher_suite_with_cipher_suite_mismatch(void) {
//   const int32_t CIPHER_SUITE_A = 0xAAAA;
//   const int32_t CIPHER_SUITE_B = 0xBBBB;
//   const int32_t CIPHER_SUITES[] = {CIPHER_SUITE_A, CIPHER_SUITE_B};
//   tst_stub_get_cipher_suites = CIPHER_SUITES;
//   tst_stub_get_cipher_suites_length =
//       sizeof(CIPHER_SUITES) / sizeof(CIPHER_SUITES[0]);
//
//   const enum edh_srv_message_1_handler_add_error_status status =
//       edh_srv_message_1_handler_add_error(
//           EDHOC_ERROR_CODE_WRONG_SELECTED_CIPHER_SUITE, &env.dummy_context,
//           "Cipher Suite Mismatch", &env.response_buffer);
//
//   TEST_ASSERT_EQUAL_MESSAGE(EDH_SRV_MSG1_ADD_ERROR_OK, status,
//                             "The error reporter should succeed when preparing
//                             " "a cipher suite mismatch payload");
//   TEST_ASSERT_EQUAL_
// }
//
// void test_add_error_fails_on_add_edhoc_to_response_failure(void) {}
