/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 12/05/2026
 * @brief Definition of the opaque struct cli_exchange
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */
#ifndef COAP_CLIENT_INTERNAL_CLI_EXCHANGE_INTERNAL_H_
#define COAP_CLIENT_INTERNAL_CLI_EXCHANGE_INTERNAL_H_

#include "coap/client/cli_exchange.h"

// ReSharper disable once CppClassNeverUsed it is used, it is opaque
struct cli_coap_exchange {
  struct cli_coap_exchange_session_data session_data;
  bool have_response;
  bool response_is_error;
  struct com_writable_buffer incoming_response_buffer;
  enum status_coap (*send_data)(struct cli_coap_exchange* exchange,
                                struct cli_coap_exchange_request request_data);

  struct {
    const uint8_t* bytes;
    size_t length;
  } internal_parsed_response;
};

#endif  // COAP_CLIENT_INTERNAL_CLI_EXCHANGE_INTERNAL_H_