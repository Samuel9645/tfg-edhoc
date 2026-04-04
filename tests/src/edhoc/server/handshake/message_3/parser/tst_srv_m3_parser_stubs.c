/**
 * @file stubs.c
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 02/04/2026
 * @brief Stubs for EDHOC server message 3 parser tests.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#include <edhoc.h>
#include <edhoc_helpers.h>

#include "edhoc/server/handshake/message_3/parser/tst_srv_m3_parser_stubs.h"

int tst_edh_srv_message_3_parser_stub_extract_result = EDHOC_SUCCESS;
bool tst_edh_srv_message_3_parser_stub_equal_result = true;

int edhoc_extract_connection_id(
    struct edhoc_extracted_fields* extracted_fields) {
  (void)extracted_fields;
  return tst_edh_srv_message_3_parser_stub_extract_result;
}

bool edhoc_connection_id_equal(const struct edhoc_connection_id* conn_id_1,
                               const struct edhoc_connection_id* conn_id_2) {
  (void)conn_id_1;
  (void)conn_id_2;
  return tst_edh_srv_message_3_parser_stub_equal_result;
}

void tst_edh_srv_message_3_parser_reset_stub_results(void) {
  tst_edh_srv_message_3_parser_stub_extract_result = EDHOC_SUCCESS;
  tst_edh_srv_message_3_parser_stub_equal_result = true;
}
