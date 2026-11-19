/*
 * Cross encryption/decryption test: every implementation encrypts and every
 * other one decrypts, on the RFC vectors and on random inputs of every
 * length from 0 to 1100 bytes (covers all block/tail combinations).
 * Exits non-zero on any mismatch.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "chacha20.h"
#include "test_vectors.h"

static const struct { const char *name; chacha20_xor_fn *fn; } impls[] = {
    { "scalar", chacha20_xor_scalar },
    { "v128",   chacha20_xor_v128   },
    { "v256",   chacha20_xor_v256   },
};
#define NUM_IMPLS (int)(sizeof(impls) / sizeof(impls[0]))
#define MAX_LEN 8192

static uint8_t in[MAX_LEN], enc[MAX_LEN], dec[MAX_LEN], ref[MAX_LEN];

int main(void)
{
    int failures = 0;

    /* 1. Encrypt with i, decrypt with j, on the RFC vectors. */
    for (int i = 0; i < NUM_IMPLS; i++) {
        for (int j = 0; j < NUM_IMPLS; j++) {
            int ok = 1;
            for (int t = 0; t < num_test_vectors; t++) {
                const test_vector_t *tv = &test_vectors_encrypt[t];
                size_t len = tv->plaintext_length;
                impls[i].fn(tv->key, tv->nonce, tv->blockcount, (const uint8_t *)tv->plaintext, enc, len);
                impls[j].fn(tv->key, tv->nonce, tv->blockcount, enc, dec, len);
                if (memcmp(enc, tv->expected_ciphertext, len) != 0 || memcmp(dec, tv->plaintext, len) != 0)
                    ok = 0;
            }
            printf("  %-6s encrypts, %-6s decrypts: %s\n", impls[i].name, impls[j].name,
                   ok ? "\033[0;32mok\033[0m" : "\033[0;31mFAIL\033[0m");
            failures += !ok;
        }
    }

    /* 2. Random key/nonce/counter/data at every length: all must give the same output. */
    srand(20241219);
    int mismatches = 0;
    for (size_t len = 0; len <= 1100; len++) {
        uint8_t key[32], nonce[12];
        for (int k = 0; k < 32; k++) key[k] = (uint8_t)rand();
        for (int k = 0; k < 12; k++) nonce[k] = (uint8_t)rand();
        uint32_t counter = (uint32_t)rand();
        for (size_t k = 0; k < len; k++) in[k] = (uint8_t)rand();

        impls[0].fn(key, nonce, counter, in, ref, len);
        for (int i = 1; i < NUM_IMPLS; i++) {
            impls[i].fn(key, nonce, counter, in, enc, len);
            if (memcmp(enc, ref, len) != 0) {
                if (mismatches++ < 5)
                    printf("  mismatch: %s vs scalar at length %zu\n", impls[i].name, len);
            }
        }
    }
    printf("  random inputs, lengths 0-1100: %s\n",
           mismatches ? "\033[0;31mFAIL\033[0m" : "\033[0;32mall implementations agree\033[0m");
    failures += mismatches != 0;

    if (failures == 0)
        printf("\033[0;32mCrossed encryption-decryption test working!\033[0m\n");
    else
        printf("\033[0;31mCrossed encryption-decryption test failed.\033[0m\n");
    return failures ? 1 : 0;
}
