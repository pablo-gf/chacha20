/*
 * 128-bit vectorized ChaCha20 (SSE2).
 *
 * Each row of the 4x4 state lives in one 128-bit register, so a single
 * vector instruction performs the same step on all four columns at once.
 * For the diagonal rounds, rows 1-3 are rotated (shuffled) so that the
 * diagonals line up as columns, then rotated back.
 */
#include <immintrin.h>
#include "chacha20.h"

#define ROTL128(v, n) _mm_or_si128(_mm_slli_epi32((v), (n)), _mm_srli_epi32((v), 32 - (n)))

/* The quarter round, applied to 4 columns in parallel. */
#define QUARTER_ROUND_X4(a, b, c, d) do {                                   \
    a = _mm_add_epi32(a, b); d = _mm_xor_si128(d, a); d = ROTL128(d, 16);   \
    c = _mm_add_epi32(c, d); b = _mm_xor_si128(b, c); b = ROTL128(b, 12);   \
    a = _mm_add_epi32(a, b); d = _mm_xor_si128(d, a); d = ROTL128(d, 8);    \
    c = _mm_add_epi32(c, d); b = _mm_xor_si128(b, c); b = ROTL128(b, 7);    \
} while (0)

/* Runs the 20 rounds on one state and writes a 64-byte keystream block. */
static void chacha20_block_v128(const uint32_t state[16], uint8_t keystream[64])
{
    const __m128i r0 = _mm_loadu_si128((const __m128i *)&state[0]);
    const __m128i r1 = _mm_loadu_si128((const __m128i *)&state[4]);
    const __m128i r2 = _mm_loadu_si128((const __m128i *)&state[8]);
    const __m128i r3 = _mm_loadu_si128((const __m128i *)&state[12]);
    __m128i v0 = r0, v1 = r1, v2 = r2, v3 = r3;

    for (int i = 0; i < 10; i++) {
        QUARTER_ROUND_X4(v0, v1, v2, v3);                   /* columns */
        v1 = _mm_shuffle_epi32(v1, _MM_SHUFFLE(0, 3, 2, 1)); /* diagonals -> columns */
        v2 = _mm_shuffle_epi32(v2, _MM_SHUFFLE(1, 0, 3, 2));
        v3 = _mm_shuffle_epi32(v3, _MM_SHUFFLE(2, 1, 0, 3));
        QUARTER_ROUND_X4(v0, v1, v2, v3);                   /* diagonals */
        v1 = _mm_shuffle_epi32(v1, _MM_SHUFFLE(2, 1, 0, 3)); /* restore order */
        v2 = _mm_shuffle_epi32(v2, _MM_SHUFFLE(1, 0, 3, 2));
        v3 = _mm_shuffle_epi32(v3, _MM_SHUFFLE(0, 3, 2, 1));
    }

    /* Add the original state back in and serialize (x86 is little-endian). */
    _mm_storeu_si128((__m128i *)&keystream[0],  _mm_add_epi32(v0, r0));
    _mm_storeu_si128((__m128i *)&keystream[16], _mm_add_epi32(v1, r1));
    _mm_storeu_si128((__m128i *)&keystream[32], _mm_add_epi32(v2, r2));
    _mm_storeu_si128((__m128i *)&keystream[48], _mm_add_epi32(v3, r3));
}

void chacha20_xor_v128(const uint8_t key[32], const uint8_t nonce[12], uint32_t counter,
                       const uint8_t *in, uint8_t *out, size_t len)
{
    uint32_t state[16];
    uint8_t keystream[64];

    chacha20_state_init(state, key, counter, nonce);

    for (size_t off = 0; off < len; off += 64) {
        size_t n = len - off < 64 ? len - off : 64;
        size_t j = 0;
        chacha20_block_v128(state, keystream);
        for (; j + 16 <= n; j += 16) {  /* XOR 16 bytes at a time */
            __m128i x = _mm_loadu_si128((const __m128i *)&in[off + j]);
            __m128i k = _mm_loadu_si128((const __m128i *)&keystream[j]);
            _mm_storeu_si128((__m128i *)&out[off + j], _mm_xor_si128(x, k));
        }
        for (; j < n; j++)              /* leftover bytes */
            out[off + j] = in[off + j] ^ keystream[j];
        state[12]++;
    }
}
