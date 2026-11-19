/*
 * Interactive encryption/decryption of user input (--enc-ci / --dec-ci).
 * The key and nonce are typed as text: their characters are used as bytes.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "chacha20.h"

/* Reads one line into buf (without the newline). Returns its length. */
static size_t read_line(const char *prompt, char *buf, size_t size)
{
    printf("\n%s\n", prompt);
    if (!fgets(buf, (int)size, stdin))
        buf[0] = '\0';
    size_t len = strcspn(buf, "\r\n");
    if (buf[len] == '\0' && len == size - 1) {    /* line too long: drop the rest */
        int c;
        while ((c = getchar()) != '\n' && c != EOF) {}
    }
    buf[len] = '\0';
    return len;
}

/* Copies the typed characters into a fixed-size key/nonce (zero-padded). */
static void read_bytes(const char *prompt, uint8_t *out, size_t n)
{
    char buf[64];
    read_line(prompt, buf, sizeof(buf));
    memset(out, 0, n);
    memcpy(out, buf, strnlen(buf, n));
}

static void read_key_nonce(uint8_t key[32], uint8_t nonce[12])
{
    read_bytes("Insert 256-bit Key (32 characters):", key, 32);
    read_bytes("Insert 96-bit Nonce (12 characters):", nonce, 12);
}

void encrypt_custom_input(chacha20_xor_fn *xor_fn)
{
    uint8_t key[32], nonce[12];
    char input[256];
    uint8_t ciphertext[256];

    read_key_nonce(key, nonce);
    size_t len = read_line("Insert Plaintext:", input, sizeof(input));

    xor_fn(key, nonce, 0, (const uint8_t *)input, ciphertext, len);

    printf("\nCiphertext (in hex):\n");
    for (size_t i = 0; i < len; i++)
        printf("%02X", ciphertext[i]);
    printf("\n");
}

void decrypt_custom_input(chacha20_xor_fn *xor_fn)
{
    uint8_t key[32], nonce[12];
    char input[514];
    uint8_t ciphertext[256];
    char plaintext[257];

    read_key_nonce(key, nonce);
    size_t hex_len = read_line("Insert Ciphertext (in hex: 8B1C...):", input, sizeof(input));

    size_t len = hex_len / 2;
    for (size_t i = 0; i < len; i++) {
        char pair[3] = { input[2 * i], input[2 * i + 1], '\0' };
        ciphertext[i] = (uint8_t)strtol(pair, NULL, 16);
    }

    xor_fn(key, nonce, 0, ciphertext, (uint8_t *)plaintext, len);
    plaintext[len] = '\0';

    printf("\nPlaintext:\n%s\n", plaintext);
}
