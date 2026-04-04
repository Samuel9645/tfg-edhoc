/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 01/04/2026
 * @brief EDHOC Message 3 operation result type to decouple EDHOC logic from
 * transport-level response mapping.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#ifndef EDHOC_SERVER_MESSAGE_3_RESULT_H
#define EDHOC_SERVER_MESSAGE_3_RESULT_H

/**
 * @brief Result codes for EDHOC server Message 3 operations.
 */
typedef enum edh_srv_message_3_handler_status {
  EDH_MSG3_HDL_OK = 0,
  EDH_MSG3_HDL_ERR_INVALID_ARGS,
  EDH_MSG3_HDL_ERR_MESSAGE_3_PROCESS_FAILED,
  EDH_MSG3_HDL_ERR_MESSAGE_4_COMPOSE_FAILED,
} edh_srv_message_3_handler_status_t;

#endif  // EDHOC_SERVER_MESSAGE_3_RESULT_H
