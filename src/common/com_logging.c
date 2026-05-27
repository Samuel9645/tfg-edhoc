/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 23/05/2026
 * @brief Definition for the logging module.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#include "../../include/common/com_logging.h"

#include <stdarg.h>
#include <stdio.h>

void com_log_error(const char* format, ...) {
  if (format == NULL) {
    fprintf(stderr, "[EDHOC_LOCAL_ERR] NULL error message provided\n");
    return;
  }
  fprintf(stderr, "[EDHOC_LOCAL_ERR] ");
  va_list args = {0};
  va_start(args, format);
  vfprintf(stderr, format, args);
  va_end(args);
}