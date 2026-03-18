#ifndef SHARED_CRYPTO_H_
#define SHARED_CRYPTO_H_

#include <psa/crypto.h>
#include <stddef.h>
#include <stdint.h>

/**
 * @brief Encrypt plaintext using AES-CCM with zero nonce
 * @param plaintext_buffer Buffer containing plaintext to encrypt
 * @param plaintext_length Length of plaintext
 * @param key_material Key material for encryption
 * @param key_length Length of key material
 * @param ciphertext_buffer Output buffer for ciphertext (must include space
 * for tag)
 * @param ciphertext_buffer_size Size of output buffer
 * @param[out] ciphertext_output_length Actual length of ciphertext
 * @return PSA_SUCCESS on success, error code otherwise
 */
psa_status_t shared_encrypt_plaintext(
    const uint8_t* plaintext_buffer, size_t plaintext_length,
    const uint8_t* key_material, size_t key_length, uint8_t* ciphertext_buffer,
    size_t ciphertext_buffer_size, size_t* ciphertext_output_length);

/**
 * @brief Decrypt ciphertext using AES-CCM with zero nonce
 * @param ciphertext_buffer Buffer containing ciphertext to decrypt
 * @param ciphertext_length Length of ciphertext (including tag)
 * @param key_material Key material for decryption
 * @param key_length Length of key material
 * @param plaintext_buffer Output buffer for plaintext
 * @param plaintext_buffer_size Size of output buffer
 * @param[out] plaintext_output_length Actual length of plaintext
 * @return PSA_SUCCESS on success, error code otherwise
 */
psa_status_t shared_decrypt_ciphertext(
    const uint8_t* ciphertext_buffer, size_t ciphertext_length,
    const uint8_t* key_material, size_t key_length, uint8_t* plaintext_buffer,
    size_t plaintext_buffer_size, size_t* plaintext_output_length);

#endif  // SHARED_CRYPTO_H_
