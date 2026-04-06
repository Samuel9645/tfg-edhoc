/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 06/04/2026
 * @brief CoAP context initialization for both server and client.
 */

#include "coap/common/cp_create_context.h"

#include "coap/config.h"

static struct cp_com_create_context_result initialization_failure(
    const enum cp_com_create_context_status status) {
  return (struct cp_com_create_context_result){
      .status = status,
      .context = NULL,
  };
}

static struct cp_com_create_context_result initialization_ok(
    coap_context_t* context) {
  return (struct cp_com_create_context_result){
      .status = CP_COM_INIT_OK,
      .context = context,
  };
}

struct cp_com_create_context_result cp_com_create_context(void) {
  coap_context_t* context = coap_new_context(NULL);
  if (context == NULL) {
    coap_log_err("failed to create CoAP context\n");
    return initialization_failure(CP_COM_INIT_ERR_CONTEXT_CREATION);
  }
  coap_context_set_block_mode(context, CP_CFG_BLOCK_MODE_LIBCOAP_DEFAULT);
  return initialization_ok(context);
}