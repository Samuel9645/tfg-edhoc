/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 22/05/2026
 * @brief Definitions for the module to generate connection identifiers.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */
#include "edhoc/common/com_generate_connection_id.h"

/**
 * @brief The range of valid CIDs for the session, using single byte ids,
 * according to RFC 9528.
 * @see [RFC
 * 9528 3.3.2](https://datatracker.ietf.org/doc/html/rfc9528#name-representation-of-byte-stri)
 */
enum {
  // WHY DON'T WE USE THIS?
  // libedhoc (v3.16) doesn't decode integers when specifying
  // EDHOC_CID_TYPE_ONE_BYTE_INTEGER, but it simply cast them, without encoding
  // it, meaning that we cannot use negative values since these are different
  // from the CBOR representation.
  COM_EDHOC_MIN_INT_CID = -24,

  COM_EDHOC_LIBEDHOC_MIN_INT_CID = 0,
  COM_EDHOC_MAX_INT_CID = 23
};

struct com_cid_generation_details {
  int8_t connection_identifier;
  const int8_t initial_value;
};

static int8_t get_next_single_byte_integer_cid(
    struct com_cid_generation_details details) {
  details.connection_identifier++;
  if (details.connection_identifier > COM_EDHOC_MAX_INT_CID) {
    details.connection_identifier = details.initial_value;
  }
  return details.connection_identifier;
}

static const int8_t INITIAL_EVEN_CID = COM_EDHOC_LIBEDHOC_MIN_INT_CID;

static struct com_cid_generation_details even_cid_counter = {
    .connection_identifier = INITIAL_EVEN_CID,
    .initial_value = INITIAL_EVEN_CID};

int8_t com_generate_even_cid(void) {
  return get_next_single_byte_integer_cid(even_cid_counter);
}

static const int8_t INITIAL_ODD_CID = COM_EDHOC_LIBEDHOC_MIN_INT_CID + 1;

static struct com_cid_generation_details odd_cid_counter = {
    .connection_identifier = INITIAL_ODD_CID, .initial_value = INITIAL_ODD_CID};

int8_t com_generate_odd_cid(void) {
  return get_next_single_byte_integer_cid(odd_cid_counter);
}