/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 18/04/2026
 * @brief Declarations for the srv_m3_process_result_builders module.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#ifndef EDHOC_SERVER_HANDSHAKE_MESSAGE_3_INTERNAL_SRV_M3_PROCESS_RESULT_BUILDERS_H_
#define EDHOC_SERVER_HANDSHAKE_MESSAGE_3_INTERNAL_SRV_M3_PROCESS_RESULT_BUILDERS_H_

#include "edhoc/server/handshake/message_3/srv_m3_process_result.h"

struct srv_edhoc_message_3_process_result
srv_edhoc_message_3_process_invalid_error_buffer_failure(void);

struct srv_edhoc_message_3_process_result srv_edhoc_message_3_process_failure(
    enum srv_edhoc_message_3_process_status status,
    struct com_readonly_buffer error_buffer);

struct srv_edhoc_message_3_process_result srv_edhoc_message_3_process_ok(void);

#endif  // EDHOC_SERVER_HANDSHAKE_MESSAGE_3_INTERNAL_SRV_M3_PROCESS_RESULT_BUILDERS_H_

