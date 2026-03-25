/**
 * @file test_edhoc_dispatch_fixture.h
 *
 * @brief Shared test fixture definitions for EDHOC dispatcher seam tests.
 *
 * Provides fake session/response models, trace state, and reusable constants
 * and mutable test inputs used by stubbed dependencies.
 */
#ifndef TEST_EDHOC_DISPATCH_FIXTURE_H_
#define TEST_EDHOC_DISPATCH_FIXTURE_H_

#include <stdbool.h>
#include <stdint.h>

#include "coap/common/config.h"
#include "coap/common/status.h"
#include "coap/server/internal/edhoc_dispatch_engine.h"

/**
 * @brief Fake response model used to capture dispatcher output code changes.
 */
typedef struct {
  /** Last response code set by the dispatcher. */
  coap_pdu_code_t last_code;
  /** Number of times response code was set. */
  size_t set_response_code_call_count;
} fake_response_t;

/**
 * @brief Fake session model exposing app-data storage.
 */
typedef struct {
  /** Application data pointer returned by session access stub. */
  void* app_data;
} fake_session_t;

/**
 * @brief Trace structure used to assert dispatcher behavior in tests.
 */
typedef struct {
  /** True when request validation is executed. */
  bool validate_request_called;
  /** Expected request content format observed by validator. */
  content_format_edhoc_values_t expected_request_format;
  /** True when response options are added. */
  bool add_response_options_called;
  /** Response content format observed by options stub. */
  content_format_edhoc_values_t response_options_format;
  /** True when Message 1 format check is executed. */
  bool check_message_1_called;
  /** True when Message 3 format check is executed. */
  bool check_message_3_called;
  /** True when Message 1 handler is executed. */
  bool handle_message_1_request_called;
  /** True when Message 3 handler is executed. */
  bool handle_message_3_request_called;
  /** True when payload add helper is executed. */
  bool add_response_payload_called;
  /** Length captured by payload add helper. */
  size_t response_payload_len;
  /** First byte captured by payload add helper. */
  uint8_t response_payload_first_byte;
  /** Second byte captured by payload add helper. */
  uint8_t response_payload_second_byte;
} dispatch_trace_t;

/** Global trace instance reset by setUp() before each test case. */
extern dispatch_trace_t global_dispatch_trace;

/**
 * @brief Fixed bytes and lengths used by test vectors and assertions.
 */
enum {
  TEST_MESSAGE_1_SUFFIX_BYTE = 0x01,
  TEST_MESSAGE_3_C_R_BYTE = 0x22,
  TEST_MESSAGE_3_SUFFIX_BYTE = 0x03,
  TEST_HANDLER_MESSAGE_BYTE = 0xAA,
  TEST_ERROR_PAYLOAD_BYTE = 0xEF,
  TEST_ZERO_PAYLOAD_LEN = 0,
  TEST_SINGLE_PAYLOAD_LEN = 1,
  TEST_DOUBLE_PAYLOAD_LEN = 2,
  TEST_SINGLE_SET_CODE_CALL = 1,
};

/** Canonical fake Message 1 payload used by validator stub. */
extern const uint8_t global_message_1_payload[];
/** Canonical fake Message 3 payload used by validator stub. */
extern const uint8_t global_message_3_payload[];
/** Current payload pointer returned by validator stub. */
extern const uint8_t* global_extracted_payload;
/** Current payload length returned by validator stub. */
extern size_t global_extracted_payload_len;
/** Current result forced by validator stub. */
extern coap_status_result_t global_validate_result;
/** Expected C_R byte used by Message 3 format checker stub. */
extern uint8_t global_expected_c_r;

#endif  // TEST_EDHOC_DISPATCH_FIXTURE_H_
