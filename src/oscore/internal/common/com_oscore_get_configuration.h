/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 26/05/2026
 * @brief Declaration of the function to get the OSCORE configuration using the
 * EDHOC context
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#ifndef OSCORE_INTERNAL_COMMON_COM_OSCORE_GET_ALGORITHMS_H_
#define OSCORE_INTERNAL_COMMON_COM_OSCORE_GET_ALGORITHMS_H_

#include <edhoc.h>

enum { COM_OSCORE_CONFIG_CSV_MAX_SIZE = 512 };

struct com_writable_char_buffer {
  char* data;
  const size_t capacity;
};

/**
 * @brief Builds a configuration buffer with the OSCORE configuration derived
 * from the given EDHOC context, including the algorithms to use for OSCORE
 * based on the cipher suite negotiated in the handshake. The configuration is
 * written in a format that can be directly used to configure OSCORE contexts
 * with the com_oscore_configure_context function.
 * @param[in] edhoc_context EDHOC context with the cipher suite negotiated and
 * configured after the handshake
 * @param[in] configuration_buffer View of the buffer to write the configuration
 * into
 * @return True on success, false on failure
 */
bool com_oscore_build_config(
    struct edhoc_context* edhoc_context,
    struct com_writable_char_buffer configuration_buffer);

#endif  // OSCORE_INTERNAL_COMMON_COM_OSCORE_GET_ALGORITHMS_H_