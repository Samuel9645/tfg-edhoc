/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 02/04/2026
 * @brief Scenario table definition for EDHOC server message 3 parser tests.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */
#ifndef EDHOC_SERVER_HANDSHAKE_MESSAGE_3_HANDLER_PARSER_SCENARIOS_H_
#define EDHOC_SERVER_HANDSHAKE_MESSAGE_3_HANDLER_PARSER_SCENARIOS_H_

#include <edhoc.h>
#include <stddef.h>
#include <stdint.h>

typedef struct tst_edh_srv_message_3_parser_test_case {
  const char* description;
  const uint8_t* request_payload;
  size_t request_length;
  const struct edhoc_context* context;
  struct edhoc_extracted_fields* extracted_fields;
} message_3_parser_test_case_t;

#endif  // EDHOC_SERVER_HANDSHAKE_MESSAGE_3_HANDLER_PARSER_SCENARIOS_H_
