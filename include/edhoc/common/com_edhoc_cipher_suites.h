/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 27/04/2026
 * @brief Declarations for pre-arranged suite packages and cipher suite structs
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#ifndef EDHOC_COMMON_COM_EDHOC_CIPHER_SUITES_H_
#define EDHOC_COMMON_COM_EDHOC_CIPHER_SUITES_H_
#include <stdbool.h>
#include <stddef.h>

struct com_edhoc_cipher_suite_details {
  const struct edhoc_cipher_suite* metadata;
  const struct edhoc_keys* (*get_keys)(void);
  const struct edhoc_crypto* (*get_crypto)(void);
};

// WHY THIS ISN'T THE FIELDS CONST?
// libedhoc (v3.16) forces this as the Message 1 processing function blindly
// selects the last cipher suite in the array as the chosen one instead of
// picking the selected_cipher_suite (which is the one that is used in the
// bindings of the cryptography methods).
// So we need to override the data of the supported list
struct com_edhoc_cipher_suite_list {
  // WHY ARE WE USING DOUBLE POINTERS?
  // This ensures that only global cipher suites are being used
  // and prevents copying errors
  const struct com_edhoc_cipher_suite_details** suites;
  size_t number_of_suites;
};

/**
 * @brief Check if the cipher suites struct is not NULL and if it contains at
 * least one valid cipher suite details struct.
 * @param list Struct containing the cipher suites details to
 * check.
 * @return true if the struct is valid, false otherwise.
 */
bool com_edhoc_cipher_suites_are_valid(struct com_edhoc_cipher_suite_list list);

extern const struct com_edhoc_cipher_suite_details COM_EDHOC_SUITE_0;
extern const struct com_edhoc_cipher_suite_list COM_EDHOC_ONLY_SUITE_0;
extern const struct com_edhoc_cipher_suite_details COM_EDHOC_SUITE_2;
extern const struct com_edhoc_cipher_suite_list COM_EDHOC_ONLY_SUITE_2;
extern const struct com_edhoc_cipher_suite_list COM_EDHOC_SUITES_2_0;

#endif  // EDHOC_COMMON_COM_EDHOC_CIPHER_SUITES_H_