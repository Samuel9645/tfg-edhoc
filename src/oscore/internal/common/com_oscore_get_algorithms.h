/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 26/05/2026
 * @brief Declaration of the function to get the algorithms from the selected
 * cipher suite into strings ready for use in OSCORE
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#ifndef OSCORE_INTERNAL_COMMON_COM_OSCORE_GET_ALGORITHMS_H_
#define OSCORE_INTERNAL_COMMON_COM_OSCORE_GET_ALGORITHMS_H_

#include <edhoc.h>

struct srv_oscore_algorithms {
  const bool success;
  const char* aead_algorithm;
  const char* hkdf_algorithm;
};

/**
 * @brief Extract the algorithms to use for OSCORE session derivation from the
 * EDHOC context, based on the selected cipher suite.
 * @param context EDHOC context to extract the selected cipher suite and
 * determine the algorithms to use for OSCORE session derivation
 * @return Struct containing the names of the algorithms to use for the OSCORE
 * session and a status of the operation
 * @see [RFC
 * 9528 10.2](https://datatracker.ietf.org/doc/html/rfc9528#name-edhoc-cipher-suites-registr)
 */
struct srv_oscore_algorithms com_oscore_get_algorithms(
    const struct edhoc_context* context);

#endif  // OSCORE_INTERNAL_COMMON_COM_OSCORE_GET_ALGORITHMS_H_