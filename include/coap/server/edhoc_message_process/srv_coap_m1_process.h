/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 01/04/2026
 * @brief CoAP adapter declarations to translate Message 1 EDHOC results into
 * CoAP response codes.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#ifndef COAP_SERVER_EDHOC_MAPPER_SRV_M1_MAPPER_H_
#define COAP_SERVER_EDHOC_MAPPER_SRV_M1_MAPPER_H_

#include <coap3/coap.h>

#include "edhoc/server/handshake/message_1/srv_m1_responder_result.h"

/**
 * @brief Process Message 1 EDHOC result (logging failures) and map it to the
 * corresponding CoAP response code.
 * @param[in] message_1_result Message 1 processing result from the EDHOC layer.
 * @return Mapped CoAP response code for the operation.
 */
coap_pdu_code_t srv_coap_process_message_1_result(
    struct srv_edhoc_message_1_responder_result message_1_result);

#endif  // COAP_SERVER_EDHOC_MAPPER_SRV_M1_MAPPER_H_
