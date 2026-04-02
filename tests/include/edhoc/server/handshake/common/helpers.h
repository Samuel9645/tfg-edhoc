/**
 * @file helpers.h
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 30/03/2026
 * @brief Constants used across EDHOC server handshake tests.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#ifndef EDHOC_SERVER_HANDSHAKE_COMMON_HELPERS_H_
#define EDHOC_SERVER_HANDSHAKE_COMMON_HELPERS_H_

#include <stdint.h>

#include "common/data_models.h"

extern const uint8_t TST_CANARY_PATTERN;
extern const uint8_t TST_NONZERO_VALUE;

com_response_buffer_t tst_edh_invalid_response(void);

void tst_edh_reset_response(com_response_buffer_t* response);

void tst_edh_srv_assert_response_clean(const com_response_buffer_t* response);

#endif  // EDHOC_SERVER_HANDSHAKE_COMMON_HELPERS_H_