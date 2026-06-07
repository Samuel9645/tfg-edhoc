#include <string.h>

#include "app/app_client.h"
#include "common/com_logging.h"
#include "common/com_parse_suites_arguments.h"
#include "edhoc/common/com_edhoc_cipher_suites.h"
#include "edhoc/common/com_edhoc_parameters.h"
#include "edhoc/common/com_generate_connection_id.h"
#include "edhoc/credentials/cred_cli.h"

static void log_usage(const char* program_name) {
  com_log_error(
      "Usage: %s -s|--supported [ <id1> <id2> ...] <server_ip>\n"
      "Where the supported suites are ordered based on preference"
      "Examples: %s -s 0 2 localhost\n"
      "          %s -s 2 192.168.10.3\n",
      program_name, program_name, program_name);
}

int main(const int argc, char* argv[]) {
  const char* program_name = argv[0];
  if (argc < 4) {
    log_usage(program_name);
    return -1;
  }
  if (strcmp(argv[1], "-s") != 0 && strcmp(argv[1], "--supported") != 0) {
    com_log_error(
        "Invalid arguments: expected '-s' or '--supported' as the first "
        "argument\n");
    log_usage(program_name);
    return -1;
  }
  const struct com_parse_suites_arguments_result parse_result =
      com_parse_suites_arguments(argv + 2, argc - 3);
  if (!parse_result.success) {
    com_log_error("Failed to parse supported suites from arguments\n");
    log_usage(program_name);
    return -1;
  }
  struct com_edhoc_create_cipher_suites_result supported_suites_result =
      com_edhoc_create_cipher_suites_from(parse_result.suites.identifiers,
                                          parse_result.suites.count);
  if (!supported_suites_result.success) {
    com_log_error(
        "Failed to create supported cipher suites from parsed identifiers\n");
    log_usage(program_name);
    com_edhoc_delete_created_cipher_suite_list(
        &supported_suites_result.cipher_suites);
    return -1;
  }

  const enum edhoc_method supported_methods[] = {EDHOC_METHOD_0};
  const struct edhoc_credentials credentials = get_client_credentials();
  const struct com_edhoc_parameters edhoc_parameters = {
      .credentials = &credentials,
      .supported_cipher_suites = supported_suites_result.cipher_suites,
      .selected_cipher_suite = supported_suites_result.cipher_suites.suites[0],
      .methods =
          {
              .data = supported_methods,
              .size = sizeof(supported_methods) / sizeof(supported_methods[0]),
          },
      .generate_connection_id = com_generate_even_cid,
  };
  const struct com_edhoc_validate_parameters_result validate_result =
      com_edhoc_validate_parameters(&edhoc_parameters);
  if (!validate_result.valid_parameters) {
    com_edhoc_delete_created_cipher_suite_list(
        &supported_suites_result.cipher_suites);
    com_log_error("Invalid EDHOC parameters\n");
    log_usage(program_name);
    return -1;
  }
  const char* server_ip = argv[argc - 1];
  const enum com_emulation_status status =
      core_run_client(edhoc_parameters, server_ip);
  com_edhoc_delete_created_cipher_suite_list(
      &supported_suites_result.cipher_suites);
  return status == COM_EMULATION_SUCCESS ? 0 : -1;
}