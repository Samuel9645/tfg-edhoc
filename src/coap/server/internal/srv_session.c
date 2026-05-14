#include "coap/server/internal/srv_session.h"

#include <edhoc_helpers.h>
#include <string.h>

enum {
  SRV_SESSION_MAX_CAPACITY = 10,
  SRV_SESSION_SEARCH_NOT_FOUND = -1,
};

static struct srv_session_entry {
  struct edhoc_connection_id connection_id;
  struct edhoc_context* context;
  bool occupied;
} session_dictionary[SRV_SESSION_MAX_CAPACITY] = {0};

static int find_index_by_cid(const struct edhoc_connection_id* cid) {
  for (int i = 0; i < SRV_SESSION_MAX_CAPACITY; i++) {
    if (session_dictionary[i].occupied &&
        edhoc_connection_id_equal(&session_dictionary[i].connection_id, cid)) {
      return i;
    }
  }
  return SRV_SESSION_SEARCH_NOT_FOUND;
}

enum srv_session_set_status srv_session_set_context_by_cid(
    const struct edhoc_connection_id* cid, struct edhoc_context* context) {
  if (cid == NULL)
    return SRV_SESSION_SET_ERR_INVALID_CID;
  if (context == NULL)
    return SRV_SESSION_SET_ERR_NULL_CONTEXT;

  if (find_index_by_cid(cid) != SRV_SESSION_SEARCH_NOT_FOUND) {
    return SRV_SESSION_SET_ERR_DUPLICATE_CID;
  }
  for (int i = 0; i < SRV_SESSION_MAX_CAPACITY; i++) {
    if (!session_dictionary[i].occupied) {
      session_dictionary[i].connection_id = *cid;
      session_dictionary[i].context = context;
      session_dictionary[i].occupied = true;
      return SRV_SESSION_SET_OK;
    }
  }
  return SRV_SESSION_SET_ERR_FULL;
}

static struct srv_session_get_result srv_session_get_ok(
    struct edhoc_context* context) {
  return (struct srv_session_get_result){.status = SRV_SESSION_GET_OK,
                                         .context = context};
}

static struct srv_session_get_result srv_session_get_failure(
    const enum srv_session_get_status status) {
  return (struct srv_session_get_result){.status = status, .context = NULL};
}

struct srv_session_get_result srv_session_get_context_by_cid(
    const struct edhoc_connection_id* cid) {
  if (cid == NULL)
    return srv_session_get_failure(SRV_SESSION_GET_ERR_INVALID_CID);

  const int index = find_index_by_cid(cid);
  if (index == SRV_SESSION_SEARCH_NOT_FOUND) {
    return srv_session_get_failure(SRV_SESSION_GET_ERR_NOT_FOUND);
  }

  return srv_session_get_ok(session_dictionary[index].context);
}

enum srv_session_remove_status srv_session_remove_context_by_cid(
    const struct edhoc_connection_id* cid) {
  if (cid == NULL)
    return SRV_SESSION_REMOVE_ERR_INVALID_CID;

  const int index = find_index_by_cid(cid);
  if (index == SRV_SESSION_SEARCH_NOT_FOUND) {
    return SRV_SESSION_REMOVE_ERR_NOT_FOUND;
  }

  session_dictionary[index].occupied = false;
  session_dictionary[index].context = NULL;
  memset(&session_dictionary[index].connection_id, 0,
         sizeof(struct edhoc_connection_id));
  return SRV_SESSION_REMOVE_OK;
}
