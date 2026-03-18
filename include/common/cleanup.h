#ifndef COMMON_CLEANUP_H_
#define COMMON_CLEANUP_H_

#include <coap3/coap.h>
#include <edhoc.h>

/**
 * @brief Holder for all resources allocated during a CoAP session, including
 * the EDHOC context. This structure allows for centralized cleanup of all
 * resources in case of errors or at the end of the session.
 */
typedef struct {
  coap_optlist_t* options;
  coap_session_t* coap_session;
  coap_context_t* coap_context;
  coap_pdu_t* pdu;
  struct edhoc_context* edhoc_current_context;
} session_resources_t;

/**
 * @brief Cleanup all resources allocated during a CoAP session, including the
 * EDHOC context.
 *
 * @param resources Pointer to the session_resources_t structure containing all
 * resources to be cleaned up.
 *
 * @note The function will check each resource for NULL before attempting to
 * free it, and will set each pointer to NULL after freeing.
 */
void cleanup_resources(session_resources_t* resources);

#endif  // COMMON_CLEANUP_H_