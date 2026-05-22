/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 27/04/2026
 * @brief Validation of edhoc parameters
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#include "edhoc/common/com_edhoc_parameters.h"

#include "edhoc/common/add_error/com_edhoc_add_internal_error.h"

static struct com_edhoc_validate_parameters_result ok(void) {
  return (struct com_edhoc_validate_parameters_result){
      .valid_parameters = true,
  };
}

static struct com_edhoc_validate_parameters_result failure(
    const struct com_readonly_buffer error_message) {
  return (struct com_edhoc_validate_parameters_result){
      .valid_parameters = false,
      .error_message = error_message,
  };
}

struct com_edhoc_validate_parameters_result invalid_error_buffer(void) {
  return (struct com_edhoc_validate_parameters_result){.valid_parameters =
                                                           false};
}

static bool suite_in_suites_list(
    const struct com_edhoc_cipher_suite_details* current_preferred_suite,
    const struct com_edhoc_cipher_suite_list* supported_suites) {
  bool found_in_supported = false;
  for (size_t j = 0; j < supported_suites->number_of_suites; j++) {
    if (current_preferred_suite == supported_suites->suites[j]) {
      found_in_supported = true;
    }
  }
  return found_in_supported;
}

bool duplicated_suites_in(
    const struct com_edhoc_cipher_suite_list* supported_suites) {
  for (size_t i = 0; i < supported_suites->number_of_suites; i++) {
    for (size_t j = i + 1; j < supported_suites->number_of_suites; j++) {
      if (supported_suites->suites[i]->metadata ==
          supported_suites->suites[j]->metadata) {
        return true;
      }
    }
  }
  return false;
}

struct com_edhoc_validate_parameters_result com_edhoc_validate_parameters(
    const struct com_edhoc_parameters* parameters,
    const struct com_writable_buffer error_buffer) {
  if (!com_writable_buffer_is_writable(error_buffer)) {
    return invalid_error_buffer();
  }
  const struct com_edhoc_cipher_suite_list* supported_suites =
      &parameters->supported_cipher_suites;
  if (!com_edhoc_cipher_suites_are_valid(*supported_suites)) {
    return failure(com_edhoc_add_internal_error_view(
        "EDHOC parameters validation error: Invalid supported cipher suites",
        error_buffer));
  }
  if (parameters->methods.data == NULL || parameters->methods.size == 0) {
    return failure(com_edhoc_add_internal_error_view(
        "EDHOC parameters validation error: Invalid EDHOC methods",
        error_buffer));
  }
  if (parameters->credentials == NULL) {
    return failure(com_edhoc_add_internal_error_view(
        "EDHOC parameters validation error: Null credentials", error_buffer));
  }
  if (parameters->selected_cipher_suite == NULL) {
    return failure(com_edhoc_add_internal_error_view(
        "EDHOC parameters validation error: Invalid selected cipher suite",
        error_buffer));
  }
  if (parameters->generate_connection_id == NULL) {
    return failure(com_edhoc_add_internal_error_view(
        "EDHOC parameters validation error: Null connection ID generator",
        error_buffer));
  }
  if (duplicated_suites_in(supported_suites)) {
    return failure(com_edhoc_add_internal_error_view(
        "EDHOC parameters validation error: Duplicated cipher "
        "suites in supported list",
        error_buffer));
  }
  if (!suite_in_suites_list(parameters->selected_cipher_suite,
                            supported_suites)) {
    return failure(com_edhoc_add_internal_error_view(
        "EDHOC parameters validation error: Selected cipher suite must be one "
        "of the supported ones",
        error_buffer));
  }
  return ok();
}
