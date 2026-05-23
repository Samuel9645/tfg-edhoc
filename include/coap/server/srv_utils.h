#ifndef COAP_SERVER_SRV_UTILS_H_
#define COAP_SERVER_SRV_UTILS_H_

#include <coap3/coap.h>

#include "coap/common/com_coap_status.h"

/**
 * @brief Resolve local interfaces and create server endpoints.
 *
 * @param[in] coap_context Active CoAP context.
 * @param[in] listen_address_string Local address to bind (for example: "::").
 * @return Status code indicating the result of the operation
 */
enum status_coap srv_coap_setup_endpoints(coap_context_t* coap_context,
                                          const char* listen_address_string);

/**
 * @brief Join a multicast group for discovery traffic.
 *
 * @param[in] coap_context Active CoAP context.
 * @param[in] multicast_address_string Multicast group address.
 * @return Status code indicating the result of the operation
 */
enum status_coap srv_coap_join_multicast_group(
    coap_context_t* coap_context, const char* multicast_address_string);

/**
 * @brief Add a POST resource and register its request handler.
 *
 * @param[in] coap_context Active CoAP context.
 * @param[in] resource_path Resource URI path.
 * @param[in] resource_handler POST handler function.
 * @return Status code indicating the result of the operation
 */
enum status_coap srv_coap_add_post_resource(
    coap_context_t* coap_context, const char* resource_path,
    coap_method_handler_t resource_handler);

/**
 * @brief Add a GET resource and register its request handler, only allowing
 * OSCORE protected requests.
 *
 * @param[in] coap_context Active CoAP context.
 * @param[in] resource_path Resource URI path.
 * @param[in] resource_handler GET handler function.
 * @return Status code indicating the result of the operation
 */
enum status_coap srv_coap_add_oscore_only_get_resource(
    coap_context_t* coap_context, const char* resource_path,
    coap_method_handler_t resource_handler);

/**
 * @brief Run the server I/O loop.
 *
 * @param[in] coap_context Active CoAP context.
 * @return Error code on failure.
 */
enum status_coap srv_coap_run_input_output_loop(coap_context_t* coap_context);

#endif  // COAP_SERVER_SRV_UTILS_H_
