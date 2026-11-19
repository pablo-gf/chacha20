/*
 * Declarations shared by every ChaCha20 implementation in this repo
 * (scalar, v128, v256): the state setup and the test/benchmark harness.
 */
#ifndef CHACHA20_COMMON_H
#define CHACHA20_COMMON_H

#include <stddef.h>
#include <stdint.h>

void state_init(uint32_t state[16], const char *constant, const uint8_t key[32], uint32_t blockcount, const uint8_t nonce[12]);

typedef struct {uint8_t key[32]; uint8_t nonce[12]; uint32_t blockcount; char plaintext[100000]; char expected_ciphertext[100000]; size_t plaintext_length;} test_vector_t;

int run_encrypt_test(test_vector_t *test);

int run_decrypt_test(test_vector_t *test);

void calculate_throughput(test_vector_t *test);

void calculate_throughput_2();

void encrypt_custom_input(char *ciphertext);

void decrypt_custom_input(char *plaintext);

#endif // CHACHA20_COMMON_H
