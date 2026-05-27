#include <stdlib.h>

#include "app/app_client.h"
#include "common/com_parse_arguments.h"
#include "edhoc/common/com_edhoc_cipher_suites.h"
#include "edhoc/common/com_edhoc_parameters.h"
#include "edhoc/common/com_generate_connection_id.h"
#include "edhoc/credentials/cred_auth.h"
#include "edhoc/credentials/cred_cli_key.h"
#include "edhoc/credentials/cred_pub_data.h"

static int client_credential_fetch(void* user_context,
                                   struct edhoc_auth_creds* credentials) {
  return cred_edhoc_auth_fetch(
      user_context, credentials, CRED_EDHOC_PUB_CLI_PK,
      CRED_EDHOC_PUB_PK_LENGTH, CRED_EDHOC_CLI_PRIVATE_KEY,
      CRED_EDHOC_CLI_PRIVATE_KEY_LENGTH, CRED_EDHOC_PUB_CLI_KID);
}

// ReSharper disable once CppParameterMayBeConstPtrOrRef (libedhoc signature
// forces it)
static int client_credential_verify(void* user_context,
                                    struct edhoc_auth_creds* credentials,
                                    const uint8_t** public_key_reference,
                                    size_t* public_key_length) {
  return cred_edhoc_auth_verify(
      user_context, credentials, CRED_EDHOC_PUB_SRV_KID, CRED_EDHOC_PUB_SRV_PK,
      CRED_EDHOC_PUB_PK_LENGTH, public_key_reference, public_key_length);
}

static const struct edhoc_credentials CREDENTIALS = {
    .fetch = client_credential_fetch,
    .verify = client_credential_verify,
};

static const enum edhoc_method SUPPORTED_METHODS_ARRAY[] = {EDHOC_METHOD_0};

static const struct com_edhoc_methods SUPPORTED_METHODS = {
    .data = SUPPORTED_METHODS_ARRAY,
    .size =
        sizeof(SUPPORTED_METHODS_ARRAY) / sizeof(SUPPORTED_METHODS_ARRAY[0]),
};

int main(const int argc, char* argv[]) {
  const struct com_parse_arguments_result parse_result =
      parse_arguments(argv, argc);
  if (!parse_result.success) {
    return EXIT_FAILURE;
  }
  struct com_edhoc_create_cipher_suites_result supported_suites_result =
      com_edhoc_create_cipher_suites_from(
          parse_result.arguments.supported_suites.identifiers,
          parse_result.arguments.supported_suites.count);
  if (!supported_suites_result.success) {
    com_edhoc_delete_created_cipher_suite_list(
        &supported_suites_result.cipher_suites);
    return EXIT_FAILURE;
  }
  struct com_edhoc_create_cipher_suites_result preferred_suites_result =
      com_edhoc_create_cipher_suites_from(
          parse_result.arguments.preferred_suites.identifiers,
          parse_result.arguments.preferred_suites.count);
  if (!preferred_suites_result.success) {
    com_edhoc_delete_created_cipher_suite_list(
        &supported_suites_result.cipher_suites);
    com_edhoc_delete_created_cipher_suite_list(
        &preferred_suites_result.cipher_suites);
    return EXIT_FAILURE;
  }
  const struct com_edhoc_parameters edhoc_parameters = {
      .credentials = &CREDENTIALS,
      .supported_cipher_suites = supported_suites_result.cipher_suites,
      .selected_cipher_suite = preferred_suites_result.cipher_suites.suites[0],
      .methods = SUPPORTED_METHODS,
      .generate_connection_id = com_generate_even_cid,
  };
  const struct com_edhoc_validate_parameters_result validate_result =
      com_edhoc_validate_parameters(&edhoc_parameters);
  if (!validate_result.valid_parameters) {
    com_edhoc_delete_created_cipher_suite_list(
        &supported_suites_result.cipher_suites);
    com_edhoc_delete_created_cipher_suite_list(
        &preferred_suites_result.cipher_suites);
    return EXIT_FAILURE;
  }

  const enum com_emulation_status status =
      core_run_client(edhoc_parameters, preferred_suites_result.cipher_suites);
  com_edhoc_delete_created_cipher_suite_list(
      &supported_suites_result.cipher_suites);
  com_edhoc_delete_created_cipher_suite_list(
      &preferred_suites_result.cipher_suites);
  return status == COM_EMULATION_SUCCESS ? EXIT_SUCCESS : EXIT_FAILURE;
}