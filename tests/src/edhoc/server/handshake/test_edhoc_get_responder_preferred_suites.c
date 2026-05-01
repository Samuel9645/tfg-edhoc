/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 01/05/2026
 * @brief Tests for getting the responder's preferred cipher suite from an error
 * buffer
 * @see [RFC
 * 9528 6.3.1](https://datatracker.ietf.org/doc/html/rfc9528/#name-cipher-suite-negotiation)
 */

#include <edhoc_crypto.h>
#include <stdint.h>
#include <unity.h>

#include "common/com_data_models.h"
#include "edhoc/client/handshake/cli_get_responder_preferred_suites.h"
#include "edhoc/client/handshake/mocks/tst_cli_mock_edhoc_process_error.h"
#include "edhoc/common/com_edhoc_cipher_suites.h"

enum {
  TST_PREF_SUITES_ERR_CODE_2 = 0x2,
  TST_PREF_SUITES_CBOR_ARRAY_3 = 0x83,
  TST_PREF_SUITES_SUITE_0 = 0x0,
  TST_PREF_SUITES_SUITE_3 = 0x3,
  TST_PREF_SUITES_SUITE_2 = 0x2,
};

void setUp(void) { tst_cli_edhoc_reset_process_error_mock(); }

/**
 * @see [RFC 9529
 * 3](https://datatracker.ietf.org/doc/html/rfc9529#name-authentication-with-static-)
 */
void test_gets_preferred_suites_from_valid_buffers(void) {
  const uint8_t ONLY_SUITE_2[] = {TST_PREF_SUITES_ERR_CODE_2,
                                  TST_PREF_SUITES_SUITE_2};
  const uint8_t ONLY_SUITE_0[] = {TST_PREF_SUITES_ERR_CODE_2,
                                  TST_PREF_SUITES_SUITE_0};
  const uint8_t SUITES_0_2_3[] = {
      TST_PREF_SUITES_ERR_CODE_2, TST_PREF_SUITES_CBOR_ARRAY_3,
      TST_PREF_SUITES_SUITE_3, TST_PREF_SUITES_SUITE_0,
      TST_PREF_SUITES_SUITE_2};

  // Extracted from [RFC
  // 9529 3.2](https://datatracker.ietf.org/doc/html/rfc9529#name-error)
  // Array format extracted from [RFC
  // 9528](https://datatracker.ietf.org/doc/html/rfc9528/#name-initiator-state-machine)
  // with the help of https://cbor.me/
  const struct {
    const uint8_t* data;
    const size_t written_length;
    const struct com_edhoc_cipher_suite_details* expected_preferred_suite;
  } test_cases[] = {{ONLY_SUITE_2, sizeof(ONLY_SUITE_2), &COM_EDHOC_SUITE_2},
                    {ONLY_SUITE_0, sizeof(ONLY_SUITE_0), &COM_EDHOC_SUITE_0},
                    {SUITES_0_2_3, sizeof(SUITES_0_2_3), &COM_EDHOC_SUITE_0}};
  const struct com_edhoc_cipher_suite_list supported_suites = {
      .suites = (struct com_edhoc_cipher_suite_details[]){COM_EDHOC_SUITE_0,
                                                          COM_EDHOC_SUITE_2},
      .number_of_suites = 2,
  };

  const size_t test_length = sizeof(test_cases) / sizeof(test_cases[0]);
  for (size_t i = 0; i < test_length; i++) {
    const struct com_readonly_buffer test_buffer = {
        .bytes = test_cases[i].data, .length = test_cases[i].written_length};

    const struct cli_edhoc_responder_preferred_suites_result result =
        cli_edhoc_get_responder_preferred_suites(&supported_suites,
                                                 test_buffer);

    TEST_ASSERT_EQUAL(CLI_EDHOC_RESP_PREFERRED_SUITES_OK, result.status);
    TEST_ASSERT_NOT_NULL(result.preferred_suite);
    TEST_ASSERT_EQUAL_INT(
        test_cases[i].expected_preferred_suite->metadata->value,
        result.preferred_suite->metadata->value);
    TEST_ASSERT_EQUAL_PTR(test_cases[i].expected_preferred_suite,
                          result.preferred_suite);
  }
}

static const uint8_t ONLY_SUITE_3[] = {TST_PREF_SUITES_ERR_CODE_2,
                                       TST_PREF_SUITES_SUITE_3};
static const struct com_readonly_buffer ONLY_SUITE_3_BUFFER = {
    .bytes = ONLY_SUITE_3, .length = sizeof(ONLY_SUITE_3)};

void test_gives_no_suites_when_negotiation_cannot_be_made(void) {
  const struct com_edhoc_cipher_suite_list supported_suites = {
      .suites = (struct com_edhoc_cipher_suite_details[]){COM_EDHOC_SUITE_0,
                                                          COM_EDHOC_SUITE_2},
      .number_of_suites = 2,
  };

  const struct cli_edhoc_responder_preferred_suites_result result =
      cli_edhoc_get_responder_preferred_suites(&supported_suites,
                                               ONLY_SUITE_3_BUFFER);

  TEST_ASSERT_EQUAL(CLI_EDHOC_RESP_PREFERRED_SUITES_NO_COMMON_SUITES,
                    result.status);
  TEST_ASSERT_NULL(result.preferred_suite);
}

void test_fails_on_invalid_data(void) {
  const struct com_readonly_buffer EMPTY_BUFFER = {0};
  const struct com_edhoc_cipher_suite_list INVALID_SUITES = {0};

  const struct {
    struct com_readonly_buffer buffer;
    struct com_edhoc_cipher_suite_list supported_suites;
    enum cli_edhoc_responder_preferred_suites_status expected_status;
  } test_cases[] = {
      {.buffer = EMPTY_BUFFER,
       .supported_suites = COM_EDHOC_ONLY_SUITE_0,
       .expected_status =
           CLI_EDHOC_RESP_PREFERRED_SUITES_ERR_EMPTY_ERROR_BUFFER},
      {.buffer = {.bytes = NULL, .length = 5},
       .supported_suites = COM_EDHOC_ONLY_SUITE_0,
       .expected_status =
           CLI_EDHOC_RESP_PREFERRED_SUITES_ERR_EMPTY_ERROR_BUFFER},
      {.buffer = ONLY_SUITE_3_BUFFER,
       .supported_suites = INVALID_SUITES,
       .expected_status =
           CLI_EDHOC_RESP_PREFERRED_SUITES_ERR_INVALID_SUPPORTED_SUITES},

      {.buffer = ONLY_SUITE_3_BUFFER,
       .supported_suites = {.number_of_suites = 1, .suites = NULL},
       .expected_status =
           CLI_EDHOC_RESP_PREFERRED_SUITES_ERR_INVALID_SUPPORTED_SUITES},

      {.buffer = ONLY_SUITE_3_BUFFER,
       .supported_suites = {.number_of_suites = 1,
                            .suites =
                                (struct com_edhoc_cipher_suite_details[]){{0}}},
       .expected_status =
           CLI_EDHOC_RESP_PREFERRED_SUITES_ERR_INVALID_SUPPORTED_SUITES},
  };

  const size_t test_length = sizeof(test_cases) / sizeof(test_cases[0]);
  for (size_t i = 0; i < test_length; i++) {
    const struct cli_edhoc_responder_preferred_suites_result result =
        cli_edhoc_get_responder_preferred_suites(
            &test_cases[i].supported_suites, test_cases[i].buffer);

    TEST_ASSERT_EQUAL(test_cases[i].expected_status, result.status);
    TEST_ASSERT_NULL(result.preferred_suite);
  }
}

void test_fails_when_error_process_fails(void) {
  tst_cli_edhoc_set_process_error_failure();

  const struct cli_edhoc_responder_preferred_suites_result result =
      cli_edhoc_get_responder_preferred_suites(&COM_EDHOC_ONLY_SUITE_0,
                                               ONLY_SUITE_3_BUFFER);

  TEST_ASSERT_EQUAL(CLI_EDHOC_RESP_PREFERRED_SUITES_ERR_PROCESSING_ERROR,
                    result.status);
  TEST_ASSERT_NULL(result.preferred_suite);
}