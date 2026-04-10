/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 01/04/2026
 * @brief CoAP adapter declarations to translate Message 3 EDHOC results into
 * CoAP response codes.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#ifndef COAP_SERVER_EDHOC_MAPPER_SRV_M3_MAPPER_H_
#define COAP_SERVER_EDHOC_MAPPER_SRV_M3_MAPPER_H_

#include <coap3/coap.h>

#include "edhoc/server/handshake/message_3/srv_m3_result.h"

/**
 * @brief Process Message 3 EDHOC result and map it to the corresponding CoAP
 * response code.
 *
 * This function logs failures using a human-readable error string and returns
 * the mapped transport response code: 2.04 (Changed) on success, 4.00 (Bad
 * Request) for protocol/request issues, or 5.00 (Internal Server Error) for
 * compose/internal failures.
 *
 * @param[in] message_3_result Message 3 processing result from the EDHOC layer.
 * @return Mapped CoAP response code for the operation.
 */
coap_pdu_code_t srv_coap_map_message_3_result_to_coap(
    enum srv_edhoc_message_3_handler_status message_3_result);

#endif  // COAP_SERVER_EDHOC_MAPPER_SRV_M3_MAPPER_H_
