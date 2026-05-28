/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 31/03/2026
 * @brief Static fixed credentials for EDHOC server and related credential
 * fetch/verify
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#ifndef EDHOC_CREDENTIALS_CRED_SRV_H_
#define EDHOC_CREDENTIALS_CRED_SRV_H_

#include <edhoc.h>

struct edhoc_credentials get_server_credentials(void);

#endif  // EDHOC_CREDENTIALS_CRED_SRV_H_
