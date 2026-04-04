/**
 * @file message_1_mapper.h
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 01/04/2026
 * @brief CoAP adapter declarations to translate Message 1 EDHOC results into
 * CoAP response codes.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#ifndef COAP_SERVER_EDHOC_MAPPER_MESSAGE_1_MAPPER_H_
#define COAP_SERVER_EDHOC_MAPPER_MESSAGE_1_MAPPER_H_

#include <coap3/coap.h>

#include "edhoc/server/handshake/message_1/srv_m1_result.h"

/**
 * @brief Process Message 1 EDHOC result and map it to the corresponding CoAP
 * response code.
 *
 * On success, this function binds the allocated EDHOC context to the CoAP
 * session app-data and returns 2.04 (Changed). On failure, it logs the mapped
 * error and returns 4.00 (Bad Request) or 5.00 (Internal Server Error)
 * depending on the result code.
 *
 * @param[in] message_1_result Message 1 processing result from the EDHOC layer.
 * @param[in,out] session CoAP session where app-data is set on success.
 * @return Mapped CoAP response code for the operation.
 */
coap_pdu_code_t cp_srv_map_message_1_result_to_coap(
    ehd_srv_message_1_handler_result_t message_1_result, coap_session_t* session);

#endif  // COAP_SERVER_EDHOC_MAPPER_MESSAGE_1_MAPPER_H_
