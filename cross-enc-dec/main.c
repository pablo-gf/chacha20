#include <stdio.h>
#include <string.h>
#include "headers/chacha20_functions.h"
#include "headers/chacha20_functions_v128.h"
#include "headers/chacha20_functions_v256.h"

/*

- This program tests the cross encryption and decryption functionalities across chacha20 and its vectorized versions.
- It utilizes a set plaintext for testing:
    "Ladies and Gentlemen of the class of '99: If I could offer you only one tip for the future, sunscreen would be it."

*/

void print_usage() {
    printf("Chacha20 cross encryption-decryption options:\n");
    printf("  *default*             Chacha20 encrypts, other versions decrypt\n");
    printf("  --enc-chacha          Chacha20 encrypts, other versions decrypt\n");
    printf("  --enc-chacha128       Chacha20 128-bit vectorization encrypts, other versions decrypt\n");
    printf("  --enc-chacha256       Chacha20 256-bit vectorization encrypts, other versions decrypt\n");
}

int main(int argc, char *argv[]) {

    char ciphertext[1024];
    char decrypted1[1024];
    char decrypted2[1024];

    uint32_t state1[16];
    uint32_t state2[16];
    
    uint32_t v0_1[4];
    uint32_t v1_1[4];
    uint32_t v2_1[4];
    uint32_t v3_1[4];

    uint32_t v0_2[8];
    uint32_t v1_2[8];
    uint32_t v2_2[8];
    uint32_t v3_2[8];
    
    uint8_t key[32] = { 
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
        0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f
        };
    uint8_t nonce[12] = {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x4a,
        0x00, 0x00, 0x00, 0x00 
        };
    uint32_t blockcount = 1;
    char plaintext[512] = {
        0x4c, 0x61, 0x64, 0x69, 0x65, 0x73, 0x20, 0x61, 0x6e, 0x64, 0x20, 0x47, 0x65, 0x6e, 0x74, 0x6c, 
        0x65, 0x6d, 0x65, 0x6e, 0x20, 0x6f, 0x66, 0x20, 0x74, 0x68, 0x65, 0x20, 0x63, 0x6c, 0x61, 0x73, 
        0x73, 0x20, 0x6f, 0x66, 0x20, 0x27, 0x39, 0x39, 0x3a, 0x20, 0x49, 0x66, 0x20, 0x49, 0x20, 0x63, 
        0x6f, 0x75, 0x6c, 0x64, 0x20, 0x6f, 0x66, 0x66, 0x65, 0x72, 0x20, 0x79, 0x6f, 0x75, 0x20, 0x6f, 
        0x6e, 0x6c, 0x79, 0x20, 0x6f, 0x6e, 0x65, 0x20, 0x74, 0x69, 0x70, 0x20, 0x66, 0x6f, 0x72, 0x20, 
        0x74, 0x68, 0x65, 0x20, 0x66, 0x75, 0x74, 0x75, 0x72, 0x65, 0x2c, 0x20, 0x73, 0x75, 0x6e, 0x73, 
        0x63, 0x72, 0x65, 0x65, 0x6e, 0x20, 0x77, 0x6f, 0x75, 0x6c, 0x64, 0x20, 0x62, 0x65, 0x20, 0x69, 
        0x74, 0x2e
        };

    int chacha_encrypts_test = 1;
    int chacha128_encrypts_test = 0;
    int chacha256_encrypts_test = 0;

    if (strcmp(argv[1], "--enc-chacha") == 0) {
        chacha_encrypts_test = 1;

    } else if (strcmp(argv[1], "--enc-chacha128") == 0) {
        chacha128_encrypts_test++;
        chacha_encrypts_test = 0;

    } else if (strcmp(argv[1], "--enc-chacha256") == 0) {
        chacha256_encrypts_test++;
        chacha_encrypts_test = 0;

    } else {
        print_usage();
        return 1;
    }

    printf("\n - Original plaintext:\n %s\n", plaintext);

    if (chacha_encrypts_test)
    {
        encrypt(state1, "expand 32-byte k", key, blockcount, nonce, plaintext, ciphertext);
        printf("\n - Encrypted with chacha20:\n ");
        for (int i = 0; i < strlen(plaintext); i++) {
            printf("%02x", (unsigned char)ciphertext[i]);
        }
        printf("\n");
        
        // Decrypt using chacha20-128-vec
        decrypt_v128(state1, "expand 32-byte k", key, blockcount, nonce, v0_1, v1_1, v2_1, v3_1, decrypted1, ciphertext);
        printf("\n - Decrypted with 128-bit vectorized chacha20:\n %s\n", decrypted1);

        // Decrypt using chacha20-256-vec
        decrypt_v256(state1, state2, "expand 32-byte k", key, blockcount, nonce, v0_2, v1_2, v2_2, v3_2, decrypted2, ciphertext);
        printf("\n - Decrypted with 256-bit vectorized chacha20:\n %s\n", decrypted2);

        // Check results
        if (strcmp(plaintext, decrypted1) == 0 && strcmp(plaintext, decrypted2) == 0) {
            printf("\033[0;32m\nCrossed encryption-decryption test working!\033[0m\n");
        } else {
            printf("\033[0;31m\nCrossed encryption-decryption test failed.\033[0m\n");
        }
    }

    if (chacha128_encrypts_test)
    {       
        encrypt_v128(state1, "expand 32-byte k", key, blockcount, nonce, v0_1, v1_1, v2_1, v3_1, plaintext, ciphertext);
        printf("\n - Encrypted with 128-bit vectorized chacha20:\n ");
        for (int i = 0; i < strlen(plaintext); i++) {
            printf("%02x", (unsigned char)ciphertext[i]);
        }
        printf("\n");
        
        decrypt(state1, "expand 32-byte k", key, blockcount, nonce, decrypted1, ciphertext);
        printf("\n - Decrypted with chacha20:\n %s\n", decrypted1);

        decrypt_v256(state1, state2, "expand 32-byte k", key, blockcount, nonce, v0_2, v1_2, v2_2, v3_2, decrypted2, ciphertext);
        printf("\n - Decrypted with 256-bit vectorized chacha20:\n %s\n", decrypted2);

        if (strcmp(plaintext, decrypted1) == 0 && strcmp(plaintext, decrypted2) == 0) {
            printf("\033[0;32m\nCrossed encryption-decryption test working!\033[0m\n");
        } else {
            printf("\033[0;31m\nCrossed encryption-decryption test failed.\033[0m\n");
        }
    }

    if (chacha256_encrypts_test)
    {
        encrypt_v256(state1, state2, "expand 32-byte k", key, blockcount, nonce, v0_2, v1_2, v2_2, v3_2, plaintext, ciphertext);
        printf("\n - Encrypted with 256-bit vectorized chacha20):\n ");
        for (int i = 0; i < strlen(plaintext); i++) {
            printf("%02x", (unsigned char)ciphertext[i]);
        }
        printf("\n");
        
        decrypt(state1, "expand 32-byte k", key, blockcount, nonce, decrypted1, ciphertext);
        printf("\n - Decrypted with chacha20:\n %s\n", decrypted1);

        decrypt_v128(state1, "expand 32-byte k", key, blockcount, nonce, v0_1, v1_1, v2_1, v3_1, decrypted2, ciphertext);
        printf("\n - Decrypted with 128-bit vectorized chacha20:\n %s\n", decrypted2);

        if (strcmp(plaintext, decrypted1) == 0 && strcmp(plaintext, decrypted2) == 0) {
            printf("\033[0;32m\nCrossed encryption/decryption test working!\033[0m\n");
        } else {
            printf("\033[0;31m\nCrossed encryption/decryption test failed.\033[0m\n");
        }
    }

    return 0;
}
