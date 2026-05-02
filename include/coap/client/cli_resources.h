#ifndef COAP_CLIENT_CLI_RESOURCES_H_
#define COAP_CLIENT_CLI_RESOURCES_H_

#include "coap/client/cli_exchange.h"
#include "edhoc/common/com_edhoc_parameters.h"

struct cli_resources {
  struct edhoc_context edhoc_context;
  struct cli_coap_exchange* exchange;
  uint8_t payload[CONFIG_COAP_MAX_PDU_SIZE];
};

struct com_writable_buffer cli_resources_get_payload(
    struct cli_resources* resources);

void cli_cleanup_resources(struct cli_resources* resources);

void cli_reset_edhoc_context(struct cli_resources* resources);

#endif  // COAP_CLIENT_CLI_RESOURCES_H_