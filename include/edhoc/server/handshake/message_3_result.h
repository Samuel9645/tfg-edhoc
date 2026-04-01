/**
 * @file message_3_result.h
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
typedef enum edhoc_server_message_3_result {
  ESHM3_OK = 0,
  ESHM3_ERR_INVALID_ARGS,
  ESHM3_ERR_MESSAGE_3_PROCESS_FAILED,
  ESHM3_ERR_MESSAGE_4_COMPOSE_FAILED,
} edhoc_server_message_3_result_t;

#endif  // EDHOC_SERVER_MESSAGE_3_RESULT_H
