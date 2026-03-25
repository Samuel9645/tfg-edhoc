#include "coap/server/test_edhoc_dispatch_fixture.h"

dispatch_trace_t global_dispatch_trace;

const uint8_t global_message_1_payload[] = {CBOR_TRUE,
                                            TEST_MESSAGE_1_SUFFIX_BYTE};
const uint8_t global_message_3_payload[] = {TEST_MESSAGE_3_C_R_BYTE,
                                            TEST_MESSAGE_3_SUFFIX_BYTE};
const uint8_t* global_extracted_payload = global_message_1_payload;
size_t global_extracted_payload_len = sizeof(global_message_1_payload);
coap_status_result_t global_validate_result = COAP_STATUS_SUCCESS;
uint8_t global_expected_c_r = TEST_MESSAGE_3_C_R_BYTE;
