<h1 align="center">ChaCha20 in C: scalar, 128-bit and 256-bit SIMD</h1>

C implementations of the ChaCha20 stream cipher ([RFC 8439](https://www.rfc-editor.org/rfc/rfc8439)): a plain scalar version and two vectorized versions using Intel intrinsics. Includes RFC test vectors, an ECRYPT-style clock-cycle benchmark, and a cross test showing that all three produce identical output.

## Layout

```
src/
  chacha20.h            public API: one function per implementation
  chacha20_common.c     state initialization (shared)
  chacha20_scalar.c     plain C, one 64-byte block at a time
  chacha20_v128.c       SSE2: each state row in one 128-bit register
  chacha20_v256.c       AVX2: two blocks processed in parallel
test/
  main.c                test + benchmark driver, built once per implementation
  test_vectors.c        RFC 8439 vectors and benchmark inputs
  custom_input.c        interactive encrypt/decrypt (--enc-ci / --dec-ci)
  cross.c               every implementation encrypts, every other decrypts
bench/                  clock-cycle benchmark scripts (.sh and .bat)
docs/                   design notes and benchmark results for each implementation
results/                benchmark spreadsheets and CSVs
```

Each implementation provides only one function:

```c
void chacha20_xor_scalar(const uint8_t key[32], const uint8_t nonce[12], uint32_t counter,
                         const uint8_t *in, uint8_t *out, size_t len);   /* also _v128, _v256 */
```

ChaCha20 XORs the input with a keystream, so the same call encrypts and decrypts. Everything else (state setup, tests, benchmark, input handling) is shared.

## Build and run

Requires `gcc` and an x86-64 CPU (`v256` needs AVX2). On Windows use MinGW (`mingw32-make`).

```bash
make                 # builds build/chacha20-{scalar,v128,v256} and build/chacha20-cross
make test            # RFC vectors for each implementation + cross test
make bench V=v256    # clock-cycle benchmark -> results/generated_results_v256.csv
make clean
```

Each `build/chacha20-<impl>` binary accepts:

| Option | What it does |
|---|---|
| `--all-tests` | All test vectors (encrypt + decrypt) and clock-cycle tests |
| `--enc-tv N` / `--dec-tv N` | Encrypt / decrypt RFC test vector N (1-5) |
| `--clock-ct N` | Clock cycles per byte for benchmark input N (1-5: 8, 64, 576, 1536, 4096 bytes) |
| `--enc-ci` / `--dec-ci` | Encrypt / decrypt your own input |

How each implementation works, and its benchmark results: [scalar](docs/scalar.md), [128-bit](docs/v128.md), [256-bit](docs/v256.md).

## CI

[`.github/workflows/ci.yml`](.github/workflows/ci.yml) runs `make test` on Linux for every push and pull request, both normally and with AddressSanitizer/UBSan. On pushes to `main` it then runs the Windows clock-cycle benchmark for each implementation and uploads `results-<impl>` CSV artifacts.

## History

The 128-bit, 256-bit and cross enc/dec versions used to live in their own repos (`chacha20-128-vec`, `chacha20-256-vec`, `chacha20-cross-enc-dec`). They were merged here with `git subtree`, so every original commit is kept. Older commits use the original file paths, so use `git log --graph` to browse them rather than filtering by folder.
