#ifndef TEST_VECTORS_H
#define TEST_VECTORS_H

#include <stddef.h>
#include <stdint.h>

typedef struct {
    uint8_t  key[32];
    uint8_t  nonce[12];
    uint32_t blockcount;
    char     plaintext[100000];
    char     expected_ciphertext[100000];
    size_t   plaintext_length;
} test_vector_t;

/* RFC 8439 vectors: encrypt plaintext -> expected_ciphertext. */
extern test_vector_t test_vectors_encrypt[];
/* Same vectors with plaintext/ciphertext swapped, to test decryption. */
extern test_vector_t test_vectors_decrypt[];
/* Inputs of the ECRYPT benchmark lengths (8, 64, 576, 1536, 4096 bytes). */
extern test_vector_t clock_cycle_tests[];

extern const int num_test_vectors;
extern const int num_clock_cycle_tests;

#endif
