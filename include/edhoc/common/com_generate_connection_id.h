/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 22/05/2026
 * @brief Declarations for the module to generate connection identifiers.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */
#ifndef EDHOC_COMMON_COM_GENERATE_CID_H_
#define EDHOC_COMMON_COM_GENERATE_CID_H_

#include <stdint.h>

/**
 * @brief Gets the next even valid one byte connection identifier of the session
 * @see [RFC
 * 9528 3.3.2](https://datatracker.ietf.org/doc/html/rfc9528#name-representation-of-byte-stri)
 * @see [RFC
 * 9528 3.3.3](https://datatracker.ietf.org/doc/html/rfc9528#name-use-of-connection-identifie)
 * @warning This is a simulation so wrapping the value is fine since the session
 * will never have more than 48 messages, but in a real implementation this
 * should be implemented in a way that guarantees uniqueness of the CID for the
 * session, as explained in RFC 9528 3.3.3.
 * @return Connection Identifier of the new session.
 */
int8_t com_generate_even_cid(void);
/**
 * @brief Gets the next odd valid one byte connection identifier of the session
 * @see [RFC
 * 9528 3.3.2](https://datatracker.ietf.org/doc/html/rfc9528#name-representation-of-byte-stri)
 * @see [RFC
 * 9528 3.3.3](https://datatracker.ietf.org/doc/html/rfc9528#name-use-of-connection-identifie)
 * @warning This is a simulation so wrapping the value is fine since the session
 * will never have more than 48 messages, but in a real implementation this
 * should be implemented in a way that guarantees uniqueness of the CID for the
 * session, as explained in RFC 9528 3.3.3.
 * @return Connection Identifier of the new session.
 */
int8_t com_generate_odd_cid(void);

#endif  // EDHOC_COMMON_COM_GENERATE_CID_H_