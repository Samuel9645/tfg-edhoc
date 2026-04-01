/**
 * @file handler.h
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 01/04/2026
 * @brief Logic for handling EDHOC Message 1 on the server side
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */
#ifndef EDHOC_SERVER_HANDSHAKE_MESSAGE_1_HANDLER_H_
#define EDHOC_SERVER_HANDSHAKE_MESSAGE_1_HANDLER_H_

#include "edhoc/server/handshake/common/request_data.h"
#include "edhoc/server/handshake/message_1/result.h"

/**
 * @brief Input data required to process EDHOC Message 1.
 */
typedef struct edh_srv_hnd_m1_request {
  /** Common session/context/response/request metadata. */
  edh_srv_hnd_com_request_t base_data;

  /** Server credentials for setting up the EDHOC context. */
  const struct edhoc_credentials* credentials;
} edh_srv_hnd_m1_request_t;

/**
 * @brief Check whether payload is properly formatted as EDHOC Message 1.
 *
 * @param[in] payload Request payload.
 * @param[in] payload_len Request payload length.
 * @return true if payload matches Message 1 framing, false otherwise.
 */
bool edh_srv_hnd_m1_is_properly_formatted(const uint8_t* payload,
                                          size_t payload_len);

/**
 * @brief Strips the CBOR TRUE prefix from the EDHOC Message 1 payload.
 * @param[in,out] payload Pointer to the buffer address; advanced by 1 byte on
 * success.
 * @param[in,out] length Pointer to the buffer length; decremented by 1 on
 * success.
 * @return EDHS_M1_OK if prefix was removed, or a EDHS_M1_ERR code if
 * invalid/missing.
 */
edh_srv_hnd_m1_status_t edh_srv_hnd_m1_remove_cbor_true_prefix(
    const uint8_t** payload, size_t* length);

/**
 * @brief Handle EDHOC Message 1 and compose Message 2.
 *
 * @param[in] message_1_request_data Session/request metadata for Message 1
 * processing, including credentials.
 * @param[out] response_data Response buffer metadata for Message 2.
 * @return Struct containing status code and allocated EDHOC context on success,
 * or error code and NULL context on failure.
 * @warning This function dynamically allocates the EDHOC context using
 * calloc(). On success, the caller assumes ownership of this memory and is
 * responsible for freeing it. On failure, the function safely cleans up after
 * itself and returns a NULL pointer.
 */
edh_srv_hnd_m1_result_t edh_srv_hnd_m1_handle(
    const edh_srv_hnd_m1_request_t* message_1_request_data,
    com_response_buffer_t* response_data);

#endif  // EDHOC_SERVER_HANDSHAKE_MESSAGE_1_HANDLER_H_
