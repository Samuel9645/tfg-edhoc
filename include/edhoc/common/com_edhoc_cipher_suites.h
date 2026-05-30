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

enum com_edhoc_cipher_suite_identifier {
  COM_EDHOC_CIPHER_SUITE_0_IDENTIFIER = 0,
  COM_EDHOC_CIPHER_SUITE_2_IDENTIFIER = 2
};

struct com_edhoc_cipher_suite_details {
  const struct edhoc_cipher_suite* metadata;
  const struct edhoc_keys* (*get_keys)(void);
  const struct edhoc_crypto* (*get_crypto)(void);
};

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

struct com_edhoc_create_cipher_suites_result {
  bool success;
  struct com_edhoc_cipher_suite_list cipher_suites;
};

/**
 * @brief Creates a list of suites with the given suites identifiers.
 * @param suites_identifiers Array of suite identifiers to create the list from.
 * @param number_of_suites Number of suites in the array.
 * @return A struct containing the cipher suite details for the given
 * identifiers, or an empty list if any of the identifiers is invalid, with a
 * success field indicating the result of the operation.
 * @warning This function uses dynamic memory allocation to create the array of
 * pointers. On failure this is freed before returning, but on success the
 * caller is responsible for freeing the data, using
 * com_edhoc_delete_created_cipher_suite_list.
 */
struct com_edhoc_create_cipher_suites_result
com_edhoc_create_cipher_suites_from(const int* suites_identifiers,
                                    size_t number_of_suites);

/**
 * @brief Frees the memory allocated for the list of cipher suites created with
 * com_edhoc_create_cipher_suites_from.
 * @param list list containing the data to be deleted
 * @warning This function should only be used when deleting a list using
 * com_edhoc_create_cipher_suites_from, as it assumes the data was allocated in
 * a specific way.
 */
void com_edhoc_delete_created_cipher_suite_list(
    struct com_edhoc_cipher_suite_list* list);

extern const struct com_edhoc_cipher_suite_details COM_EDHOC_SUITE_0;
extern const struct com_edhoc_cipher_suite_list COM_EDHOC_ONLY_SUITE_0;
extern const struct com_edhoc_cipher_suite_details COM_EDHOC_SUITE_2;
extern const struct com_edhoc_cipher_suite_list COM_EDHOC_ONLY_SUITE_2;
extern const struct com_edhoc_cipher_suite_list COM_EDHOC_SUITES_2_0;

#endif  // EDHOC_COMMON_COM_EDHOC_CIPHER_SUITES_H_