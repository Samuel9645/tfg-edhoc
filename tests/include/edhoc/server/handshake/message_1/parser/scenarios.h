/**
 * @file scenarios.h
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 02/04/2026
 * @brief Test scenarios for EDHOC server message 1 parser tests.
 */

#ifndef TST_EDH_SRV_MESSAGE_1_PARSER_SCENARIOS_H
#define TST_EDH_SRV_MESSAGE_1_PARSER_SCENARIOS_H

#include <stddef.h>
#include <stdint.h>

#include "common/data_models.h"

/**
 * @struct message_1_parser_test_case_t
 * @brief Test case definition for message 1 parser invalid-data scenarios.
 *
 * @member description Human-readable test case description for assertion
 * messages.
 * @member payload Pointer to payload buffer; may be NULL to test error
 * handling.
 * @member length Length of payload buffer.
 * @member output Pointer to output buffer; may be NULL to test error handling.
 */
typedef struct {
  const char* description;
  const uint8_t* payload;
  size_t length;
  com_readonly_buffer_t* output;
} message_1_parser_test_case_t;

#endif /* TST_EDH_SRV_MESSAGE_1_PARSER_SCENARIOS_H */
