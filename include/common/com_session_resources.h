#ifndef COMMON_COM_SESSION_RESOURCES_H_
#define COMMON_COM_SESSION_RESOURCES_H_

#include <coap3/coap.h>

struct com_session_resources {
  coap_context_t* coap_context;
};

/**
 * @brief Cleanup all resources allocated during a CoAP session, including the
 * EDHOC context.
 *
 * @param[in] resources Pointer to the struct com_session_resources structure
 * containing all resources to be cleaned up.
 *
 * @note The function will check each resource for NULL before attempting to
 * free it, and will set each pointer to NULL after freeing.
 */
void com_cleanup_resources(struct com_session_resources* resources);

#endif  // COMMON_COM_SESSION_RESOURCES_H_