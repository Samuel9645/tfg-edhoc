/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 31/03/2026
 * @brief Definitions for the module to get public data credentials
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#include "edhoc/credentials/cred_pub_data.h"

static const uint8_t CLI_SUITE_2_PUBLIC_KEY[] = {
    0x04, 0x8d, 0x26, 0x81, 0xba, 0xc7, 0x3b, 0xa5, 0xcf, 0x68, 0xd8,
    0xca, 0xf8, 0x1e, 0x41, 0xfe, 0x22, 0xaf, 0xa6, 0x5e, 0x69, 0x31,
    0x35, 0x84, 0x92, 0xda, 0xc5, 0x85, 0x70, 0xd9, 0xfd, 0x4c, 0xe5,
    0x1a, 0xcc, 0xc3, 0x67, 0x13, 0xc6, 0x0d, 0x21, 0x1d, 0xcb, 0x43,
    0xdf, 0x51, 0x25, 0xef, 0x70, 0xb4, 0x13, 0xbc, 0x3a, 0x2c, 0x8a,
    0xcf, 0xb9, 0xae, 0xb5, 0x3a, 0xe4, 0x81, 0x39, 0x56, 0x71};

static const uint8_t SRV_SUITE_2_PUBLIC_KEY[] = {
    0x04, 0x78, 0x28, 0xdd, 0x49, 0x29, 0xc8, 0x90, 0x84, 0x36, 0xa6,
    0xaf, 0xeb, 0x94, 0x9f, 0xa0, 0x94, 0xaf, 0xcd, 0xc7, 0x41, 0x9f,
    0x51, 0x9d, 0x7f, 0x81, 0x5b, 0x38, 0x20, 0x2d, 0xa7, 0x87, 0x25,
    0x84, 0x75, 0x65, 0x42, 0x03, 0x40, 0x6b, 0x7d, 0x06, 0xc2, 0xfb,
    0xec, 0x31, 0x9f, 0xab, 0x95, 0x3b, 0x87, 0xeb, 0xe2, 0xce, 0xdb,
    0xc7, 0xe2, 0x93, 0x59, 0x49, 0x29, 0x62, 0x24, 0x4d, 0x32};

static const int32_t SRV_SUITE_2_KID = 20;
static const int32_t CLI_SUITE_2_KID = 18;

// Last 32 bytes of the private key

static const uint8_t CLI_SUITE_0_PUBLIC_KEY[] = {
    0xee, 0xf7, 0x52, 0x2e, 0x29, 0xdb, 0x09, 0x3c, 0x74, 0xc1, 0xb3,
    0x65, 0x57, 0xf5, 0x20, 0x98, 0x8a, 0x18, 0xc8, 0x61, 0xce, 0xe4,
    0xac, 0x66, 0xf7, 0x84, 0xdf, 0xfa, 0x05, 0xeb, 0xa0, 0x98};

static const uint8_t SRV_SUITE_0_PUBLIC_KEY[] = {
    0x4c, 0x8e, 0x1c, 0xc6, 0x51, 0xe9, 0x41, 0x2d, 0x73, 0x74, 0x8c,
    0x02, 0x0e, 0xc0, 0x39, 0x0f, 0xd5, 0xbd, 0x86, 0x50, 0x8d, 0x34,
    0xe0, 0x73, 0xb9, 0xb9, 0xc1, 0x36, 0x5a, 0xb2, 0xb5, 0xf4};

static const int32_t SRV_SUITE_0_KID = 21;
static const int32_t CLI_SUITE_0_KID = 19;

struct cred_get_edhoc_public_data_result cred_get_client_public_data(
    const enum com_edhoc_cipher_suite_identifier cipher_suite_identifier) {
  switch (cipher_suite_identifier) {
  case COM_EDHOC_CIPHER_SUITE_2_IDENTIFIER:
    return (struct cred_get_edhoc_public_data_result){
        .credentials_found = true,
        .public_credentials = {
            .key_id = CLI_SUITE_2_KID,
            .public_key =
                {
                    .key = CLI_SUITE_2_PUBLIC_KEY,
                    .length = sizeof(CLI_SUITE_2_PUBLIC_KEY),
                },
        }};
  case COM_EDHOC_CIPHER_SUITE_0_IDENTIFIER:
    return (struct cred_get_edhoc_public_data_result){
        .credentials_found = true,
        .public_credentials = {
            .key_id = CLI_SUITE_0_KID,
            .public_key =
                {
                    .key = CLI_SUITE_0_PUBLIC_KEY,
                    .length = sizeof(CLI_SUITE_0_PUBLIC_KEY),
                },
        }};
  default:
    return (struct cred_get_edhoc_public_data_result){
        .credentials_found = false,
    };
  }
}

struct cred_get_edhoc_public_data_result cred_get_server_public_data(
    const enum com_edhoc_cipher_suite_identifier cipher_suite_identifier) {
  switch (cipher_suite_identifier) {
  case COM_EDHOC_CIPHER_SUITE_2_IDENTIFIER:
    return (struct cred_get_edhoc_public_data_result){
        .credentials_found = true,
        .public_credentials = {
            .key_id = SRV_SUITE_2_KID,
            .public_key =
                {
                    .key = SRV_SUITE_2_PUBLIC_KEY,
                    .length = sizeof(SRV_SUITE_2_PUBLIC_KEY),
                },
        }};
  case COM_EDHOC_CIPHER_SUITE_0_IDENTIFIER:
    return (struct cred_get_edhoc_public_data_result){
        .credentials_found = true,
        .public_credentials = {
            .key_id = SRV_SUITE_0_KID,
            .public_key =
                {
                    .key = SRV_SUITE_0_PUBLIC_KEY,
                    .length = sizeof(SRV_SUITE_0_PUBLIC_KEY),
                },
        }};
  default:
    return (struct cred_get_edhoc_public_data_result){
        .credentials_found = false,
    };
  }
}