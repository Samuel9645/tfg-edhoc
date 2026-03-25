#ifndef COAP_SERVER_UTILS_H_
#define COAP_SERVER_UTILS_H_

#include <coap3/coap.h>

#include "coap/common/status.h"

/**
 * @brief Initialize libcoap and create a server context with configured block
 * mode.
 *
 * @return Pointer to created CoAP context on success, NULL on failure.
 *
 * @note On failure, any allocated resources are freed and libcoap is cleaned
 * up.
 */
coap_context_t* coap_server_create_context(void);

/**
 * @brief Resolve local interfaces and create server endpoints.
 *
 * @param[in] coap_context Active CoAP context.
 * @param[in] listen_address_string Local address to bind (for example: "::").
 * @return COAP_SERVER_UTILS_SUCCESS on success, COAP_SERVER_UTILS_ERROR on
 * failure.
 */
coap_status_result_t coap_server_setup_endpoints(
    coap_context_t* coap_context, const char* listen_address_string);

/**
 * @brief Join a multicast group for discovery traffic.
 *
 * @param[in] coap_context Active CoAP context.
 * @param[in] multicast_address_string Multicast group address.
 * @return COAP_SERVER_UTILS_SUCCESS on success, COAP_SERVER_UTILS_ERROR on
 * failure.
 */
coap_status_result_t coap_server_join_multicast_group(
    coap_context_t* coap_context, const char* multicast_address_string);

/**
 * @brief Add a GET resource and register its request handler.
 *
 * @param[in] coap_context Active CoAP context.
 * @param[in] resource_path Resource URI path.
 * @param[in] resource_handler GET handler function.
 * @return COAP_SERVER_UTILS_SUCCESS on success, COAP_SERVER_UTILS_ERROR on
 * failure.
 */
coap_status_result_t coap_server_add_post_resource(
    coap_context_t* coap_context, const char* resource_path,
    coap_method_handler_t resource_handler);

/**
 * @brief Run the server I/O loop.
 *
 * @param[in] coap_context Active CoAP context.
 * @return COAP_SERVER_UTILS_SUCCESS on graceful stop, COAP_SERVER_UTILS_ERROR
 * on I/O failure.
 */
coap_status_result_t coap_server_run_input_output_loop(
    coap_context_t* coap_context);

#endif // COAP_SERVER_UTILS_H_
