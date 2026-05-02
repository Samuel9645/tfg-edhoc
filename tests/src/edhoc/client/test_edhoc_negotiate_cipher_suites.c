/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 01/05/2026
 * @brief Module to negotiate the new suites to include in the Message 1
 * @see [RFC
 * 9528 6.3.1](https://datatracker.ietf.org/doc/html/rfc9528/#name-cipher-suite-negotiation)
 */

#include <edhoc_cipher_suite_0.h>
#include <edhoc_crypto.h>
#include <stdint.h>
#include <unity.h>

#include "common/com_data_models.h"
#include "edhoc/client/handshake/cli_negotiate_cipher_suites.h"
#include "edhoc/client/handshake/mocks/tst_cli_mock_edhoc_process_error.h"
#include "edhoc/common/com_edhoc_cipher_suites.h"

// Array format extracted from [RFC
// 9528](https://datatracker.ietf.org/doc/html/rfc9528/#name-initiator-state-machine)
// with the help of https://cbor.me/
enum {
  TST_PREF_SUITES_ERR_CODE_2 = 0x2,
  TST_PREF_SUITES_CBOR_ARRAY_3 = 0x83,
  TST_PREF_SUITES_SUITE_0 = 0x0,
  TST_PREF_SUITES_SUITE_3 = 0x3,
  TST_PREF_SUITES_SUITE_2 = 0x2,
};

// WHY DO WE NEED THIS?
// libedhoc only supports cipher suites 0 and 2. Since we want to verify order
// preservation so we need a dummy suite just for testing the behavior
static const struct edhoc_cipher_suite DUMMY_SUITE = {
    .value = 4,
    .aead_key_length = 16,
    .aead_tag_length = 8,
    .aead_iv_length = 13,
    .hash_length = 32,
    .mac_length = 8,
    .ecc_key_length = 32,
    .ecc_sign_length = 64,
};
const struct com_edhoc_cipher_suite_details DUMMY_SUITE_DETAILS = {
    .metadata = &DUMMY_SUITE,
    .get_crypto = edhoc_cipher_suite_0_get_crypto,
    .get_keys = edhoc_cipher_suite_0_get_keys,
};

static const struct com_edhoc_cipher_suite_details* DUMMY_SUITE_DETAILS_PTRS[] =
    {&DUMMY_SUITE_DETAILS};
const struct com_edhoc_cipher_suite_list DUMMY_SUITE_LIST = {
    .number_of_suites =
        sizeof(DUMMY_SUITE_DETAILS_PTRS) / sizeof(DUMMY_SUITE_DETAILS_PTRS[0]),
    .suites = DUMMY_SUITE_DETAILS_PTRS,
};

void setUp(void) { tst_cli_edhoc_reset_process_error_mock(); }

static struct com_edhoc_cipher_suite_list get_cipher_suites_0_2_dummy_list(
    void) {
  static const struct com_edhoc_cipher_suite_details*
      CIPHER_SUITE_0_2_DUMMY_ARR[] = {&COM_EDHOC_SUITE_0, &COM_EDHOC_SUITE_2,
                                      &DUMMY_SUITE_DETAILS};

  static const struct com_edhoc_cipher_suite_list CIPHER_SUITE_0_2_DUMMY = {
      .suites = CIPHER_SUITE_0_2_DUMMY_ARR,
      .number_of_suites = sizeof(CIPHER_SUITE_0_2_DUMMY_ARR) /
                          sizeof(CIPHER_SUITE_0_2_DUMMY_ARR[0]),
  };
  return CIPHER_SUITE_0_2_DUMMY;
}

void assert_suite_details_are_equal(
    const struct com_edhoc_cipher_suite_details* expected_suite,
    const struct com_edhoc_cipher_suite_details* current_suite) {
  TEST_ASSERT_NOT_NULL(expected_suite);
  TEST_ASSERT_NOT_NULL(current_suite);
  TEST_ASSERT_EQUAL_INT32(current_suite->metadata->value,
                          expected_suite->metadata->value);
  TEST_ASSERT_EQUAL_PTR(current_suite, expected_suite);
}

static void assert_renegotiation_contains_initial_preferences(
    const struct com_edhoc_cipher_suite_list initial_preferred_suites,
    const struct com_edhoc_cipher_suite_details* const* renegotiation_suites) {
  for (size_t j = 0; j < initial_preferred_suites.number_of_suites; j++) {
    assert_suite_details_are_equal(initial_preferred_suites.suites[j],
                                   renegotiation_suites[j]);
  }
}

static void assert_last_suite_is_the_preferred_suite(
    const struct cli_edhoc_suites_negotiation_result result,
    const struct com_edhoc_cipher_suite_details* expected_suite) {
  TEST_ASSERT_GREATER_THAN_size_t(0,
                                  result.renegotiation_suites.number_of_suites);
  const size_t last_index = result.renegotiation_suites.number_of_suites - 1;
  assert_suite_details_are_equal(
      expected_suite, result.renegotiation_suites.suites[last_index]);
}
static const struct com_edhoc_cipher_suite_list PREFER_DUMMY_CIPHER_SUITE =
    DUMMY_SUITE_LIST;

/**
 * @see [RFC 9529
 * 3](https://datatracker.ietf.org/doc/html/rfc9529#name-authentication-with-static-)
 */
void test_gets_preferred_suites_from_valid_buffers(void) {
  const struct com_edhoc_cipher_suite_list PREFER_CIPHER_SUITE_0 =
      COM_EDHOC_ONLY_SUITE_0;
  const uint8_t ONLY_SUITE_2[] = {TST_PREF_SUITES_ERR_CODE_2,
                                  TST_PREF_SUITES_SUITE_2};
  const struct com_edhoc_cipher_suite_list PREFER_CIPHER_SUITE_2_DUMMY = {
      .suites =
          (const struct com_edhoc_cipher_suite_details*[]){&COM_EDHOC_SUITE_2},

      .number_of_suites = 1};

  const uint8_t ONLY_SUITE_0[] = {TST_PREF_SUITES_ERR_CODE_2,
                                  TST_PREF_SUITES_SUITE_0};

  const uint8_t SUITES_0_2_3[] = {
      TST_PREF_SUITES_ERR_CODE_2, TST_PREF_SUITES_CBOR_ARRAY_3,
      TST_PREF_SUITES_SUITE_3, TST_PREF_SUITES_SUITE_0,
      TST_PREF_SUITES_SUITE_2};

  // Extracted from [RFC
  // 9529 3.2](https://datatracker.ietf.org/doc/html/rfc9529#name-error)
  const struct {
    const uint8_t* data;
    const size_t written_length;
    const struct com_edhoc_cipher_suite_list own_initial_preferred_suites;
    const struct com_edhoc_cipher_suite_details* expected_preferred_suite;
  } test_cases[] = {{ONLY_SUITE_2, sizeof(ONLY_SUITE_2), PREFER_CIPHER_SUITE_0,
                     &COM_EDHOC_SUITE_2},
                    {ONLY_SUITE_0, sizeof(ONLY_SUITE_0),
                     PREFER_CIPHER_SUITE_2_DUMMY, &COM_EDHOC_SUITE_0},
                    {SUITES_0_2_3, sizeof(SUITES_0_2_3),
                     PREFER_DUMMY_CIPHER_SUITE, &COM_EDHOC_SUITE_0}};

  const size_t test_length = sizeof(test_cases) / sizeof(test_cases[0]);
  for (size_t i = 0; i < test_length; i++) {
    const struct com_readonly_buffer test_buffer = {
        .bytes = test_cases[i].data, .length = test_cases[i].written_length};
    const struct com_edhoc_cipher_suite_list initial_preferred_suites =
        test_cases[i].own_initial_preferred_suites;

    const struct cli_edhoc_suites_negotiation_result result =
        cli_edhoc_negotiate_suites(get_cipher_suites_0_2_dummy_list(),
                                   initial_preferred_suites, test_buffer);

    TEST_ASSERT_EQUAL(CLI_EDHOC_NEGOTIATE_SUITES_OK, result.status);
    TEST_ASSERT_EQUAL_size_t(initial_preferred_suites.number_of_suites + 1,
                             result.renegotiation_suites.number_of_suites);
    assert_renegotiation_contains_initial_preferences(
        initial_preferred_suites, result.renegotiation_suites.suites);
    assert_last_suite_is_the_preferred_suite(
        result, test_cases[i].expected_preferred_suite);
  }
}

void test_prioritize_initiator_suite_preference(void) {
  // This case seems imposible but since libedhoc only support cipher suites 0
  // and 2, and we are only checking that the order is preserved, is valid
  const uint8_t SUITES_3_2_0[] = {
      TST_PREF_SUITES_ERR_CODE_2, TST_PREF_SUITES_CBOR_ARRAY_3,
      TST_PREF_SUITES_SUITE_3, TST_PREF_SUITES_SUITE_2,
      TST_PREF_SUITES_SUITE_0};
  const struct com_readonly_buffer test_buffer = {
      .bytes = SUITES_3_2_0, .length = sizeof(SUITES_3_2_0)};
  const struct com_edhoc_cipher_suite_list own_supported_suites =
      get_cipher_suites_0_2_dummy_list();
  const struct com_edhoc_cipher_suite_list initial_preferred_suites =
      PREFER_DUMMY_CIPHER_SUITE;

  const struct cli_edhoc_suites_negotiation_result result =
      cli_edhoc_negotiate_suites(own_supported_suites, initial_preferred_suites,
                                 test_buffer);

  TEST_ASSERT_EQUAL(CLI_EDHOC_NEGOTIATE_SUITES_OK, result.status);
  TEST_ASSERT_EQUAL_size_t(initial_preferred_suites.number_of_suites + 1,
                           result.renegotiation_suites.number_of_suites);
  assert_renegotiation_contains_initial_preferences(
      initial_preferred_suites, result.renegotiation_suites.suites);
  assert_last_suite_is_the_preferred_suite(result, &COM_EDHOC_SUITE_0);
}

void assert_result_empty(
    const struct cli_edhoc_suites_negotiation_result result) {
  TEST_ASSERT_EQUAL_size_t(0, result.renegotiation_suites.number_of_suites);
}

static const uint8_t ONLY_SUITE_3[] = {TST_PREF_SUITES_ERR_CODE_2,
                                       TST_PREF_SUITES_SUITE_3};
static const struct com_readonly_buffer ONLY_SUITE_3_BUFFER = {
    .bytes = ONLY_SUITE_3, .length = sizeof(ONLY_SUITE_3)};

void test_gives_no_suites_when_negotiation_cannot_be_made(void) {
  const struct cli_edhoc_suites_negotiation_result result =
      cli_edhoc_negotiate_suites(get_cipher_suites_0_2_dummy_list(),
                                 PREFER_DUMMY_CIPHER_SUITE,
                                 ONLY_SUITE_3_BUFFER);

  TEST_ASSERT_EQUAL(CLI_EDHOC_NEGOTIATE_SUITES_NO_COMMON_SUITES, result.status);
  assert_result_empty(result);
}

void test_fails_on_invalid_data(void) {
  const struct com_readonly_buffer EMPTY_BUFFER = {0};
  const struct com_edhoc_cipher_suite_list INVALID_SUITES = {0};

  const struct {
    struct com_readonly_buffer buffer;
    struct com_edhoc_cipher_suite_list supported_suites;
    const struct com_edhoc_cipher_suite_list initial_preferred_suites;
    enum cli_edhoc_suites_negotiation_status expected_status;
  } test_cases[] = {
      {.buffer = EMPTY_BUFFER,
       .supported_suites = COM_EDHOC_ONLY_SUITE_0,
       .initial_preferred_suites = DUMMY_SUITE_LIST,
       .expected_status = CLI_EDHOC_NEGOTIATE_SUITES_ERR_EMPTY_ERROR_BUFFER},
      {.buffer = {.bytes = NULL, .length = 5},
       .supported_suites = COM_EDHOC_ONLY_SUITE_0,
       .initial_preferred_suites = DUMMY_SUITE_LIST,
       .expected_status = CLI_EDHOC_NEGOTIATE_SUITES_ERR_EMPTY_ERROR_BUFFER},
      {.buffer = ONLY_SUITE_3_BUFFER,
       .supported_suites = INVALID_SUITES,
       .initial_preferred_suites = DUMMY_SUITE_LIST,
       .expected_status =
           CLI_EDHOC_NEGOTIATE_SUITES_ERR_INVALID_SUPPORTED_SUITES},

      {.buffer = ONLY_SUITE_3_BUFFER,
       .supported_suites = {.number_of_suites = 1, .suites = NULL},
       .initial_preferred_suites = DUMMY_SUITE_LIST,
       .expected_status =
           CLI_EDHOC_NEGOTIATE_SUITES_ERR_INVALID_SUPPORTED_SUITES},

      {.buffer = ONLY_SUITE_3_BUFFER,
       .supported_suites =
           {.number_of_suites = 1,
            .suites = (const struct com_edhoc_cipher_suite_details*[]){NULL}},
       .initial_preferred_suites = DUMMY_SUITE_LIST,
       .expected_status =
           CLI_EDHOC_NEGOTIATE_SUITES_ERR_INVALID_SUPPORTED_SUITES},
      {.buffer = ONLY_SUITE_3_BUFFER,
       .initial_preferred_suites = INVALID_SUITES,
       .supported_suites = COM_EDHOC_ONLY_SUITE_0,
       .expected_status =
           CLI_EDHOC_NEGOTIATE_SUITES_ERR_INVALID_INITIAL_PREFERRED_SUITES},

      {.buffer = ONLY_SUITE_3_BUFFER,
       .initial_preferred_suites = {.number_of_suites = 1, .suites = NULL},
       .supported_suites = COM_EDHOC_ONLY_SUITE_0,
       .expected_status =
           CLI_EDHOC_NEGOTIATE_SUITES_ERR_INVALID_INITIAL_PREFERRED_SUITES},

      {.buffer = ONLY_SUITE_3_BUFFER,
       .initial_preferred_suites =
           {.number_of_suites = 1,
            .suites = (const struct com_edhoc_cipher_suite_details*[]){NULL}},
       .supported_suites = COM_EDHOC_ONLY_SUITE_0,
       .expected_status =
           CLI_EDHOC_NEGOTIATE_SUITES_ERR_INVALID_INITIAL_PREFERRED_SUITES},
  };

  const size_t test_length = sizeof(test_cases) / sizeof(test_cases[0]);
  for (size_t i = 0; i < test_length; i++) {
    const struct cli_edhoc_suites_negotiation_result result =
        cli_edhoc_negotiate_suites(test_cases[i].supported_suites,
                                   test_cases[i].initial_preferred_suites,
                                   test_cases[i].buffer);

    TEST_ASSERT_EQUAL(test_cases[i].expected_status, result.status);
    assert_result_empty(result);
  }
}

void test_fails_when_error_process_fails(void) {
  tst_cli_edhoc_set_process_error_failure();

  const struct cli_edhoc_suites_negotiation_result result =
      cli_edhoc_negotiate_suites(COM_EDHOC_ONLY_SUITE_0, DUMMY_SUITE_LIST,
                                 ONLY_SUITE_3_BUFFER);

  TEST_ASSERT_EQUAL(CLI_EDHOC_NEGOTIATE_SUITES_ERR_PROCESSING_ERROR,
                    result.status);
  assert_result_empty(result);
}