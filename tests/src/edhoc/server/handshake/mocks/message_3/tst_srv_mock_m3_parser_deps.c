/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 02/04/2026
 * @brief Stubs for EDHOC server message 3 parser tests.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#include "edhoc/server/handshake/mocks/message_3/tst_srv_mock_m3_parser_deps.h"

#include <edhoc.h>
#include <edhoc_helpers.h>

int tst_srv_edhoc_message_3_parser_stub_extract_result = EDHOC_SUCCESS;
bool tst_srv_edhoc_message_3_parser_stub_equal_result = true;
struct edhoc_extracted_fields tst_srv_edhoc_message_3_extracted_fields_result =
    {0};

void tst_srv_edhoc_message_3_parser_reset_stub_results(void) {
  tst_srv_edhoc_message_3_parser_stub_extract_result = EDHOC_SUCCESS;
  tst_srv_edhoc_message_3_parser_stub_equal_result = true;
  tst_srv_edhoc_message_3_extracted_fields_result =
      (struct edhoc_extracted_fields){0};
}

void tst_srv_m3_parser_set_extraction_failure(void) {
  tst_srv_edhoc_message_3_parser_stub_extract_result =
      EDHOC_ERROR_GENERIC_ERROR;
}

void tst_srv_m3_parser_set_cid_mismatch(void) {
  tst_srv_edhoc_message_3_parser_stub_equal_result = false;
}

void tst_srv_m3_parser_set_success_data(
    const struct edhoc_extracted_fields fields) {
  tst_srv_edhoc_message_3_extracted_fields_result = fields;
}

int edhoc_extract_connection_id(
    struct edhoc_extracted_fields* extracted_fields) {
  if (tst_srv_edhoc_message_3_parser_stub_extract_result == EDHOC_SUCCESS &&
      extracted_fields != NULL) {
    *extracted_fields = tst_srv_edhoc_message_3_extracted_fields_result;
  }
  return tst_srv_edhoc_message_3_parser_stub_extract_result;
}

bool edhoc_connection_id_equal(const struct edhoc_connection_id* conn_id_1,
                               const struct edhoc_connection_id* conn_id_2) {
  (void)conn_id_1;
  (void)conn_id_2;
  return tst_srv_edhoc_message_3_parser_stub_equal_result;
}
