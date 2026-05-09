/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 11/04/2026
 * @brief Definitions for the com_set_error_info module.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */
#include "edhoc/common/add_error/internal/com_edhoc_set_error_info.h"

#include <string.h>

void com_edhoc_set_error_info(struct edhoc_error_info* error_info,
                              const char* error_message) {
  memset(error_info, 0, sizeof(*error_info));
  const size_t error_message_length = strlen(error_message);
  // WHY DO WE DO THIS CAST?
  // libedhoc (v3.16) marks the text_string as a regular char* but it does not modify it
  // in any of the usages, so this may be an oversight of the library
  error_info->text_string = (char*)error_message;
  error_info->total_entries = error_message_length;
  error_info->written_entries = error_message_length;
}
