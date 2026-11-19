#include <stdint.h>
#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "chacha20_common.h"

void quarter_round(uint32_t state[], int a, int b, int c, int d);

void columns(uint32_t state[]);	

void diagonals(uint32_t state[]);

void permute_state(uint32_t state[16], uint8_t output_keystream[64]);

void encrypt(uint32_t state[16], const char *constant, const uint8_t key[32], uint32_t blockcount, const uint8_t nonce[12], char *plaintext, char *ciphertext, unsigned long plaintext_length);

void decrypt(uint32_t state1[16], const char *constant, const uint8_t key[32], uint32_t blockcount, const uint8_t nonce[12], char *output_plaintext, char *ciphertext, unsigned long ciphertext_length);

#endif // CHACHA20_FUNCTIONS_H