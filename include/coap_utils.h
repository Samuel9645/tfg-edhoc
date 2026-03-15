#ifndef COAP_UTILS_H
#define COAP_UTILS_H

#include <stddef.h>
#include <stdint.h>

#include "edhoc_context.h"

/**
 * Export the EDHOC shared secret used by client/server payload protection.
 *
 * @param[in] context Active EDHOC context.
 * @param[in] role_name Caller role for logs (for example: "Client" or
 * "Server").
 * @param[out] shared_secret Output secret buffer.
 * @param[in] shared_secret_size Size of output buffer.
 * @return EDHOC_SUCCESS on success, negative or EDHOC error code on failure.
 */
int export_edhoc_shared_secret(struct edhoc_context* context,
                               const char* role_name, uint8_t* shared_secret,
                               size_t shared_secret_size);

#endif
