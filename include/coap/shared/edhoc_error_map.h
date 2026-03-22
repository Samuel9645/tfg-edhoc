#ifndef COAP_SHARED_EDHOC_ERROR_MAP_H_
#define COAP_SHARED_EDHOC_ERROR_MAP_H_

#include <coap3/coap.h>
#include <edhoc.h>
#include <stdbool.h>

/**
 * @brief Map EDHOC failure to CoAP response code and log details.
 *
 * @param[in] edhoc_ctx EDHOC context carrying the failure details.
 * @param[in] operation_label Label used in error logs.
 * @param[in] protocol_error true for protocol errors, false for internal
 * errors.
 * @param[in,out] response CoAP response PDU where code is set.
 */
void coap_shared_map_edhoc_failure_to_response(struct edhoc_context* edhoc_ctx,
                                               const char* operation_label,
                                               bool protocol_error,
                                               coap_pdu_t* response);

#endif  // COAP_SHARED_EDHOC_ERROR_MAP_H_
