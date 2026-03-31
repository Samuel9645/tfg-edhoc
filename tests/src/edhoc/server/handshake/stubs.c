/**
 * @file stubs.c
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since Tue 31 Mar
 * @brief Stubs to link to when testing
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#include <edhoc.h>

/**
 * @brief Global variables to control the behavior of the stubs in tests. Tests
 * can set these variables to simulate different scenarios and verify how the
 * code under test responds to various EDHOC library outcomes.
 */
const int DEFAULT_STUB_RESULT = EDHOC_SUCCESS;

int stub_edhoc_setup_res = DEFAULT_STUB_RESULT;
int stub_edhoc_process_res = DEFAULT_STUB_RESULT;
int stub_edhoc_compose_res = DEFAULT_STUB_RESULT;

// 2. Implement the functions the linker is looking for
int edhoc_common_setup_context(struct edhoc_context* context,
                               const struct edhoc_credentials* credentials) {
  (void)context;
  (void)credentials;
  return stub_edhoc_setup_res;
}

int edhoc_message_1_process(struct edhoc_context* context,
                            const uint8_t* message, size_t length) {
  (void)context;
  (void)message;
  (void)length;
  return stub_edhoc_process_res;
}

int edhoc_message_2_compose(struct edhoc_context* context,
                            uint8_t* response_buffer, size_t capacity,
                            size_t* length) {
  (void)context;
  (void)response_buffer;
  (void)capacity;
  (void)length;
  return stub_edhoc_compose_res;
}