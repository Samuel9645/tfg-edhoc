/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 06/04/2026
 * @brief CoAP context initialization for both server and client.
 */
#ifndef COAP_COMMON_COAP_CONTEXT_H_
#define COAP_COMMON_COAP_CONTEXT_H_
#include <coap3/coap.h>

enum com_coap_create_context_status {
  COM_COAP_INIT_OK,
  COM_COAP_INIT_ERR_CONTEXT_CREATION
};

struct com_coap_create_context_result {
  const enum com_coap_create_context_status status;
  coap_context_t *context;
};

/**
 * @brief Creates the context and applies all the necessary options
 * @return Struct containing the status of the operation and the created context
 * on success, NULL context on failure.
 */
struct com_coap_create_context_result com_coap_create_context(void);

#endif  // COAP_COMMON_COAP_CONTEXT_H_