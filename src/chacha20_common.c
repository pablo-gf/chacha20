#include "chacha20.h"

static uint32_t load32_le(const uint8_t *p)
{
    return (uint32_t)p[0] | (uint32_t)p[1] << 8 | (uint32_t)p[2] << 16 | (uint32_t)p[3] << 24;
}

void chacha20_state_init(uint32_t state[16], const uint8_t key[32], uint32_t counter, const uint8_t nonce[12])
{
    static const uint8_t constant[16] = "expand 32-byte k";

    for (int i = 0; i < 4; i++)
        state[i] = load32_le(constant + 4 * i);
    for (int i = 0; i < 8; i++)
        state[4 + i] = load32_le(key + 4 * i);
    state[12] = counter;
    for (int i = 0; i < 3; i++)
        state[13 + i] = load32_le(nonce + 4 * i);
}
