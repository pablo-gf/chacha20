/*
 * Scalar ChaCha20: operates on the 16-word state one word at a time.
 */
#include "chacha20.h"

#define ROTL32(x, n) (((x) << (n)) | ((x) >> (32 - (n))))

/* Mixes 4 words of the state with add, XOR and rotate (ARX). */
static void quarter_round(uint32_t s[16], int a, int b, int c, int d)
{
    s[a] += s[b]; s[d] ^= s[a]; s[d] = ROTL32(s[d], 16);
    s[c] += s[d]; s[b] ^= s[c]; s[b] = ROTL32(s[b], 12);
    s[a] += s[b]; s[d] ^= s[a]; s[d] = ROTL32(s[d], 8);
    s[c] += s[d]; s[b] ^= s[c]; s[b] = ROTL32(s[b], 7);
}

/* Runs the 20 rounds on one state and serializes it into a 64-byte keystream block. */
static void chacha20_block(const uint32_t input[16], uint8_t keystream[64])
{
    uint32_t s[16];
    for (int i = 0; i < 16; i++)
        s[i] = input[i];

    for (int i = 0; i < 10; i++) {      /* 10 double rounds = 20 rounds */
        quarter_round(s, 0, 4,  8, 12); /* columns */
        quarter_round(s, 1, 5,  9, 13);
        quarter_round(s, 2, 6, 10, 14);
        quarter_round(s, 3, 7, 11, 15);
        quarter_round(s, 0, 5, 10, 15); /* diagonals */
        quarter_round(s, 1, 6, 11, 12);
        quarter_round(s, 2, 7,  8, 13);
        quarter_round(s, 3, 4,  9, 14);
    }

    for (int i = 0; i < 16; i++) {
        uint32_t w = s[i] + input[i];   /* add the original state back in */
        keystream[4 * i + 0] = (uint8_t)(w);
        keystream[4 * i + 1] = (uint8_t)(w >> 8);
        keystream[4 * i + 2] = (uint8_t)(w >> 16);
        keystream[4 * i + 3] = (uint8_t)(w >> 24);
    }
}

void chacha20_xor_scalar(const uint8_t key[32], const uint8_t nonce[12], uint32_t counter,
                         const uint8_t *in, uint8_t *out, size_t len)
{
    uint32_t state[16];
    uint8_t keystream[64];

    chacha20_state_init(state, key, counter, nonce);

    for (size_t off = 0; off < len; off += 64) {
        size_t n = len - off < 64 ? len - off : 64;
        chacha20_block(state, keystream);
        for (size_t j = 0; j < n; j++)
            out[off + j] = in[off + j] ^ keystream[j];
        state[12]++;                    /* next block */
    }
}
