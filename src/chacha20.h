/*
 * ChaCha20 stream cipher (RFC 8439): public interface.
 *
 * Three implementations of the same function are provided:
 *   chacha20_xor_scalar  plain C, one 64-byte block at a time
 *   chacha20_xor_v128    128-bit SIMD (SSE2), one state row per vector
 *   chacha20_xor_v256    256-bit SIMD (AVX2), two blocks in parallel
 *
 * ChaCha20 XORs the input with a keystream, so the same call encrypts
 * and decrypts.
 */
#ifndef CHACHA20_H
#define CHACHA20_H

#include <stddef.h>
#include <stdint.h>

/* in and out may be the same buffer. len can be any size (no padding needed). */
typedef void chacha20_xor_fn(const uint8_t key[32], const uint8_t nonce[12], uint32_t counter,
                             const uint8_t *in, uint8_t *out, size_t len);

chacha20_xor_fn chacha20_xor_scalar;
chacha20_xor_fn chacha20_xor_v128;
chacha20_xor_fn chacha20_xor_v256;

/*
 * Shared by all implementations: builds the 4x4 initial state.
 *   row 0: the constant "expand 32-byte k"
 *   rows 1-2: the 256-bit key
 *   row 3: block counter + 96-bit nonce
 * All words are read little-endian.
 */
void chacha20_state_init(uint32_t state[16], const uint8_t key[32], uint32_t counter, const uint8_t nonce[12]);

#endif /* CHACHA20_H */
