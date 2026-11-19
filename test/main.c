/*
 * Test and benchmark driver, shared by all implementations.
 *
 * The same source is compiled once per implementation with -DIMPL=scalar,
 * -DIMPL=v128 or -DIMPL=v256, so each binary calls its implementation
 * directly (no function pointer in the timed code).
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <x86intrin.h>
#include "chacha20.h"
#include "test_vectors.h"

#ifndef IMPL
#error "Build with -DIMPL=scalar, -DIMPL=v128 or -DIMPL=v256"
#endif
#define CONCAT_(a, b) a##b
#define CONCAT(a, b) CONCAT_(a, b)
#define chacha20_xor CONCAT(chacha20_xor_, IMPL)

#define GREEN "\033[0;32m"
#define RED   "\033[0;31m"
#define RESET "\033[0m"

void encrypt_custom_input(chacha20_xor_fn *xor_fn);
void decrypt_custom_input(chacha20_xor_fn *xor_fn);

static void print_usage(void)
{
    printf("Chacha20 Options:\n");
    printf("  --all-tests           All tests\n");
    printf("  --enc-tv N            Encrypt test vector N\n");
    printf("  --dec-tv N            Decrypt test vector N\n");
    printf("  --clock-ct N          Run clock cycles test\n");
    printf("  --enc-ci              Encrypt custom input\n");
    printf("  --dec-ci              Decrypt custom input\n");
}

/*
 * Runs one vector, prints its clock cycles per byte, and returns 1 if the
 * output matches. Encryption and decryption are the same operation in
 * ChaCha20; the decrypt vectors simply have plaintext/ciphertext swapped.
 */
static int run_test(const test_vector_t *test)
{
    size_t len = test->plaintext_length;
    uint8_t *output = malloc(len ? len : 1);
    if (!output) {
        perror("malloc");
        exit(1);
    }

    unsigned long long start = __rdtsc();
    chacha20_xor(test->key, test->nonce, test->blockcount, (const uint8_t *)test->plaintext, output, len);
    unsigned long long cycles = __rdtsc() - start;

    printf("Total input bytes: %zu", len);
    printf("\nClock cycles per byte: %.2f\n", (double)cycles / len);

    int ok = memcmp(output, test->expected_ciphertext, len) == 0;
    free(output);
    return ok;
}

/* Parses the N argument (1-based) and checks it is in range. */
static int parse_index(int argc, char *argv[], int count)
{
    int n = argc > 2 ? atoi(argv[2]) : 0;
    if (n < 1 || n > count) {
        fprintf(stderr, "%s needs a number from 1 to %d\n", argv[1], count);
        exit(1);
    }
    return n - 1;
}

static int run_vectors(test_vector_t *vectors, const char *what)
{
    int passed = 0;
    for (int i = 0; i < num_test_vectors; i++) {
        printf("TEST VECTOR %d:\n", i + 1);
        if (run_test(&vectors[i])) {
            printf(GREEN "%s successful for test vector %i!" RESET "\n\n", what, i + 1);
            passed++;
        } else {
            printf(RED "%s for test vector %i failed." RESET "\n\n", what, i + 1);
        }
    }
    printf("%d/%d %s tests passed.\n", passed, num_test_vectors, what);
    return passed == num_test_vectors;
}

int main(int argc, char *argv[])
{
    if (argc < 2) {
        print_usage();
        return 1;
    }
    const char *opt = argv[1];

    if (strcmp(opt, "--all-tests") == 0) {
        printf("\n---------------ENCRYPTION TESTS---------------\n\n");
        int ok = run_vectors(test_vectors_encrypt, "Encryption");
        printf("\n---------------DECRYPTION TESTS---------------\n\n");
        ok &= run_vectors(test_vectors_decrypt, "Decryption");
        printf("\n---------------CLOCK CYCLES---------------\n\n");
        for (int i = 0; i < num_clock_cycle_tests; i++) {
            printf("TEST %d:\n", i + 1);
            run_test(&clock_cycle_tests[i]);
            printf("\n");
        }
        return ok ? 0 : 1;
    }

    if (strcmp(opt, "--enc-tv") == 0 || strcmp(opt, "--dec-tv") == 0) {
        int enc = opt[2] == 'e';
        int i = parse_index(argc, argv, num_test_vectors);
        const char *what = enc ? "Encryption" : "Decryption";
        printf("TEST VECTOR %d:\n", i + 1);
        int ok = run_test(enc ? &test_vectors_encrypt[i] : &test_vectors_decrypt[i]);
        if (ok)
            printf(GREEN "%s successful for test vector %i!" RESET "\n\n", what, i + 1);
        else
            printf(RED "%s for test vector %i failed." RESET "\n\n", what, i + 1);
        return ok ? 0 : 1;
    }

    if (strcmp(opt, "--clock-ct") == 0) {
        int i = parse_index(argc, argv, num_clock_cycle_tests);
        printf("CLOCK CYCLE TEST %d:\n", i + 1);
        run_test(&clock_cycle_tests[i]);
        printf("\n");
        return 0;
    }

    if (strcmp(opt, "--enc-ci") == 0) {
        encrypt_custom_input(chacha20_xor);
        return 0;
    }

    if (strcmp(opt, "--dec-ci") == 0) {
        decrypt_custom_input(chacha20_xor);
        return 0;
    }

    print_usage();
    return 1;
}
