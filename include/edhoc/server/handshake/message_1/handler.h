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
typedef struct edhoc_server_message_1_request_data {
  /** Common session/context/response/request metadata. */
  edhoc_server_common_request_data_t base_data;

  /** Server credentials for setting up the EDHOC context. */
  const struct edhoc_credentials* credentials;
} edhoc_server_message_1_request_data_t;

/**
 * @brief Check whether payload is properly formatted as EDHOC Message 1.
 *
 * @param[in] payload Request payload.
 * @param[in] payload_len Request payload length.
 * @return true if payload matches Message 1 framing, false otherwise.
 */
bool edhoc_server_is_properly_formatted_message_1(const uint8_t* payload,
                                                  size_t payload_len);

/**
 * @brief Strips the CBOR TRUE prefix from the EDHOC Message 1 payload.
 * @param[in,out] payload Pointer to the buffer address; advanced by 1 byte on
 * success.
 * @param[in,out] length Pointer to the buffer length; decremented by 1 on
 * success.
 * @return ESHM1_OK if prefix was removed, or a ESHM1_ERR code if
 * invalid/missing.
 */
edhoc_server_message_1_status_t edhoc_server_remove_cbor_true_prefix(
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
edhoc_server_message_1_result_t edhoc_server_handle_message_1(
    const edhoc_server_message_1_request_data_t* message_1_request_data,
    common_response_buffer_t* response_data);

#endif  // EDHOC_SERVER_HANDSHAKE_MESSAGE_1_HANDLER_H_
