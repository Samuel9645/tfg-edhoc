/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 02/04/2026
 * @brief Stubs for EDHOC server message 3 extract tests.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#ifndef EDHOC_SERVER_HANDSHAKE_MESSAGE_3_HANDLER_EXTRACT_STUBS_H_
#define EDHOC_SERVER_HANDSHAKE_MESSAGE_3_HANDLER_EXTRACT_STUBS_H_

#include <edhoc_helpers.h>

void tst_srv_edhoc_message_3_extract_reset_stub_results(void);

void tst_srv_m3_extract_set_extraction_failure(void);
void tst_srv_m3_extract_set_cid_mismatch(void);
void tst_srv_m3_extract_set_success_data(struct edhoc_extracted_fields fields);

#endif  // EDHOC_SERVER_HANDSHAKE_MESSAGE_3_HANDLER_EXTRACT_STUBS_H_
