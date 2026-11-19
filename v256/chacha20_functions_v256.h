#ifndef CHACHA20_FUNCTIONS_V256_H
#define CHACHA20_FUNCTIONS_V256_H

# include <stdio.h>
# include <string.h>
# include <stdint.h>
# include <immintrin.h>
# include "chacha20_common.h"

void state_to_vectors_v256(uint32_t state[16], uint32_t state2[16], uint32_t v0[8], uint32_t v1[8], uint32_t v2[8], uint32_t v3[8]);

void double_whole_round_v256(uint32_t *v0, uint32_t *v1, uint32_t *v2, uint32_t *v3);

void permute_state_v256(uint32_t state1[16], uint32_t state2[16], uint32_t *v0, uint32_t *v1, uint32_t *v2, uint32_t *v3, uint8_t keystream[128]);

void vectors_to_state_v256(uint32_t state1[16], uint32_t state2[16], __m256i v0, __m256i v1, __m256i v2, __m256i v3);

void encrypt_v256(uint32_t state1[16], uint32_t state2[16], const char *constant, const uint8_t key[32], uint32_t blockcount, const uint8_t nonce[12], uint32_t *v0, uint32_t *v1, uint32_t *v2, uint32_t *v3, char *plaintext, char *ciphertext, unsigned long plaintext_length);

void decrypt_v256(uint32_t state1[16], uint32_t state2[16], const char *constant, const uint8_t key[32], uint32_t blockcount, const uint8_t nonce[12], uint32_t *v0, uint32_t *v1, uint32_t *v2, uint32_t *v3, char *plaintext, char *ciphertext, unsigned long ciphertext_length);

#endif // CHACHA20_FUNCTIONS_V256_H
