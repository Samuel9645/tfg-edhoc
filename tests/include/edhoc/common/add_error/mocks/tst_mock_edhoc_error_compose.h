/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 11/04/2026
 * @brief Mocked function edhoc_message_error_compose
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#ifndef EDHOC_COMMON_ADD_ERROR_TST_MOCK_EDHOC_ERROR_COMPOSE_H_
#define EDHOC_COMMON_ADD_ERROR_TST_MOCK_EDHOC_ERROR_COMPOSE_H_

void srv_edhoc_reset_error_compose_mock(void);

void srv_edhoc_use_real_compose(void);

void srv_edhoc_set_error_compose_failed(void);

#endif  // EDHOC_COMMON_ADD_ERROR_TST_MOCK_EDHOC_ERROR_COMPOSE_H_