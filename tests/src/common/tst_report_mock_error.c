/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 08/04/2026
 * @brief Helper function to write a message if a mock fails
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#include "common/tst_report_mock_error.h"

#include <stdio.h>

void tst_report_mock_error(const char* message) {
  fprintf(stderr, "\n!!! MOCK FAILURE: %s !!!\n", message);
  fflush(stderr);
}
