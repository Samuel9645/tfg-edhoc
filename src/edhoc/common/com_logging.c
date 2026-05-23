/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 23/05/2026
 * @brief Definition for the logging module.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#include "edhoc/common/com_logging.h"

#include <stdio.h>

void com_edhoc_log_error(const char* message) {
  if (message == NULL) {
    fprintf(stderr, "[EDHOC_LOCAL_ERR] NULL error message provided\n");
  }
  fprintf(stderr, "[EDHOC_LOCAL_ERR] %s\n", message);
}