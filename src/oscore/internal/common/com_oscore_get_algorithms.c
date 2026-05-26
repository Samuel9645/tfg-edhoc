/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 26/05/2026
 * @brief Definition of the function to get the algorithms from the selected
 * cipher suite into strings ready for use in OSCORE
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#include "oscore/internal/common/com_oscore_get_algorithms.h"

struct srv_oscore_algorithms com_oscore_get_algorithms(
    const struct edhoc_context* context) {
  switch (context->private_csuite[context->private_chosen_csuite_idx].value) {
  case 0:
  case 2:
    return (struct srv_oscore_algorithms){
        .aead_algorithm = "AES-CCM-16-64-128",
        .hkdf_algorithm = "direct+HKDF-SHA-256",
        .success = true};
  default:
    return (struct srv_oscore_algorithms){0};
  }
}