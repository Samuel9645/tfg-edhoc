/**
 * @file result.c
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 01/04/2026
 * @brief EDHOC Message 1 result constructors.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */
#include "edhoc/server/handshake/message_1/result.h"

#include <stddef.h>

edh_srv_hnd_m1_result_t edh_srv_hnd_m1_ok(struct edhoc_context* edhoc_ctx) {
  return (edh_srv_hnd_m1_result_t){.status = EDH_SERV_HND_M1_OK,
                                   .edhoc_ctx = edhoc_ctx};
}

edh_srv_hnd_m1_result_t edh_srv_hnd_m1_failure(
    const edh_srv_hnd_m1_status_t status) {
  return (edh_srv_hnd_m1_result_t){.status = status, .edhoc_ctx = NULL};
}