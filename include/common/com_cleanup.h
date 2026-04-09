#ifndef COMMON_COM_CLEANUP_H_
#define COMMON_COM_CLEANUP_H_

#include <coap3/coap.h>
#include <edhoc.h>

struct com_session_resources {
  coap_optlist_t* options;
  coap_session_t* coap_session;
  coap_context_t* coap_context;
  coap_pdu_t* pdu;
  struct edhoc_context* edhoc_current_context;
};

/**
 * @brief Cleanup all resources allocated during a CoAP session, including the
 * EDHOC context.
 *
 * @param resources Pointer to the struct com_session_resources structure
 * containing all resources to be cleaned up.
 *
 * @note The function will check each resource for NULL before attempting to
 * free it, and will set each pointer to NULL after freeing.
 */
void com_cleanup_resources(struct com_session_resources* resources);

#endif  // COMMON_COM_CLEANUP_H_