/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 02/04/2026
 * @brief Stubs for EDHOC server message 3 parser tests.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#ifndef EDHOC_SERVER_HANDSHAKE_MESSAGE_3_HANDLER_PARSER_STUBS_H_
#define EDHOC_SERVER_HANDSHAKE_MESSAGE_3_HANDLER_PARSER_STUBS_H_

#include <stdbool.h>

void tst_edh_srv_message_3_parser_reset_stub_results(void);

extern int tst_edh_srv_message_3_parser_stub_extract_result;
extern bool tst_edh_srv_message_3_parser_stub_equal_result;
extern struct edhoc_extracted_fields
    tst_edh_srv_message_3_extracted_fields_result;

#endif  // EDHOC_SERVER_HANDSHAKE_MESSAGE_3_HANDLER_PARSER_STUBS_H_
