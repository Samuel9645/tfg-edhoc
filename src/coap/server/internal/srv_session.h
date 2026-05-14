/**
 * @file
 * @brief Internal EDHOC session dictionary.
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 14/05/2026
 */

#ifndef COAP_SERVER_INTERNAL_SRV_SESSION_H_
#define COAP_SERVER_INTERNAL_SRV_SESSION_H_

#include <edhoc.h>

enum srv_session_set_status {
  SRV_SESSION_SET_OK = 0,
  SRV_SESSION_SET_ERR_FULL,
  SRV_SESSION_SET_ERR_DUPLICATE_CID,
  SRV_SESSION_SET_ERR_NULL_CONTEXT,
  SRV_SESSION_SET_ERR_INVALID_CID,
};

enum srv_session_get_status {
  SRV_SESSION_GET_OK = 0,
  SRV_SESSION_GET_ERR_NOT_FOUND,
  SRV_SESSION_GET_ERR_INVALID_CID,
};

struct srv_session_get_result {
  const enum srv_session_get_status status;
  struct edhoc_context* context;
};

enum srv_session_remove_status {
  SRV_SESSION_REMOVE_OK = 0,
  SRV_SESSION_REMOVE_ERR_NOT_FOUND,
  SRV_SESSION_REMOVE_ERR_INVALID_CID,
};

enum srv_session_set_status srv_session_set_context_by_cid(
    const struct edhoc_connection_id* cid, struct edhoc_context* context);

struct srv_session_get_result srv_session_get_context_by_cid(
    const struct edhoc_connection_id* cid);

enum srv_session_remove_status srv_session_remove_context_by_cid(
    const struct edhoc_connection_id* cid);

#endif  // COAP_SERVER_INTERNAL_SRV_SESSION_H_
