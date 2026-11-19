/*
 * 256-bit vectorized ChaCha20 (AVX2).
 *
 * Same idea as the 128-bit version, but each 256-bit register holds the
 * same row of TWO consecutive blocks (counter n in the low half, n+1 in
 * the high half), so every instruction works on two blocks at once and
 * each iteration produces 128 bytes of keystream.
 */
#include <immintrin.h>
#include "chacha20.h"

#define ROTL256(v, n) _mm256_or_si256(_mm256_slli_epi32((v), (n)), _mm256_srli_epi32((v), 32 - (n)))

/* The quarter round, applied to 4 columns of 2 blocks in parallel. */
#define QUARTER_ROUND_X8(a, b, c, d) do {                                        \
    a = _mm256_add_epi32(a, b); d = _mm256_xor_si256(d, a); d = ROTL256(d, 16);  \
    c = _mm256_add_epi32(c, d); b = _mm256_xor_si256(b, c); b = ROTL256(b, 12);  \
    a = _mm256_add_epi32(a, b); d = _mm256_xor_si256(d, a); d = ROTL256(d, 8);   \
    c = _mm256_add_epi32(c, d); b = _mm256_xor_si256(b, c); b = ROTL256(b, 7);   \
} while (0)

/* Loads row r of the state into both halves, with the counters n and n+1 in row 3. */
static __m256i load_row_x2(const uint32_t state[16], int r)
{
    __m128i row = _mm_loadu_si128((const __m128i *)&state[4 * r]);
    return _mm256_set_m128i(row, row);
}

/* Runs the 20 rounds on blocks n and n+1 and writes 128 bytes of keystream. */
static void chacha20_block_v256(const uint32_t state[16], uint8_t keystream[128])
{
    const __m256i r0 = load_row_x2(state, 0);
    const __m256i r1 = load_row_x2(state, 1);
    const __m256i r2 = load_row_x2(state, 2);
    const __m256i r3 = _mm256_add_epi32(load_row_x2(state, 3),
                                        _mm256_set_epi32(0, 0, 0, 1, 0, 0, 0, 0)); /* counter+1 in high half */
    __m256i v0 = r0, v1 = r1, v2 = r2, v3 = r3;

    for (int i = 0; i < 10; i++) {
        QUARTER_ROUND_X8(v0, v1, v2, v3);                      /* columns */
        v1 = _mm256_shuffle_epi32(v1, _MM_SHUFFLE(0, 3, 2, 1)); /* diagonals -> columns */
        v2 = _mm256_shuffle_epi32(v2, _MM_SHUFFLE(1, 0, 3, 2));
        v3 = _mm256_shuffle_epi32(v3, _MM_SHUFFLE(2, 1, 0, 3));
        QUARTER_ROUND_X8(v0, v1, v2, v3);                      /* diagonals */
        v1 = _mm256_shuffle_epi32(v1, _MM_SHUFFLE(2, 1, 0, 3)); /* restore order */
        v2 = _mm256_shuffle_epi32(v2, _MM_SHUFFLE(1, 0, 3, 2));
        v3 = _mm256_shuffle_epi32(v3, _MM_SHUFFLE(0, 3, 2, 1));
    }

    v0 = _mm256_add_epi32(v0, r0);
    v1 = _mm256_add_epi32(v1, r1);
    v2 = _mm256_add_epi32(v2, r2);
    v3 = _mm256_add_epi32(v3, r3);

    /* Low halves form block n (bytes 0-63), high halves block n+1 (bytes 64-127). */
    _mm256_storeu_si256((__m256i *)&keystream[0],  _mm256_permute2x128_si256(v0, v1, 0x20));
    _mm256_storeu_si256((__m256i *)&keystream[32], _mm256_permute2x128_si256(v2, v3, 0x20));
    _mm256_storeu_si256((__m256i *)&keystream[64], _mm256_permute2x128_si256(v0, v1, 0x31));
    _mm256_storeu_si256((__m256i *)&keystream[96], _mm256_permute2x128_si256(v2, v3, 0x31));
}

void chacha20_xor_v256(const uint8_t key[32], const uint8_t nonce[12], uint32_t counter,
                       const uint8_t *in, uint8_t *out, size_t len)
{
    uint32_t state[16];
    uint8_t keystream[128];

    chacha20_state_init(state, key, counter, nonce);

    for (size_t off = 0; off < len; off += 128) {
        size_t n = len - off < 128 ? len - off : 128;
        size_t j = 0;
        chacha20_block_v256(state, keystream);
        for (; j + 32 <= n; j += 32) {  /* XOR 32 bytes at a time */
            __m256i x = _mm256_loadu_si256((const __m256i *)&in[off + j]);
            __m256i k = _mm256_loadu_si256((const __m256i *)&keystream[j]);
            _mm256_storeu_si256((__m256i *)&out[off + j], _mm256_xor_si256(x, k));
        }
        for (; j < n; j++)              /* leftover bytes */
            out[off + j] = in[off + j] ^ keystream[j];
        state[12] += 2;                 /* two blocks per iteration */
    }
}
