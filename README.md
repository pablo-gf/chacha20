<h1 align="center">ChaCha20 in C: scalar, 128-bit and 256-bit SIMD</h1>

C implementations of the ChaCha20 stream cipher ([RFC 8439](https://www.rfc-editor.org/rfc/rfc8439)), from a plain scalar version to vectorized versions using Intel intrinsics, with test vectors, a clock-cycle benchmark and a cross-implementation test.

| Folder | What it is |
|---|---|
| [`scalar/`](scalar) | Reference implementation, one block at a time |
| [`v128/`](v128) | 128-bit SIMD (SSE): one state row per vector |
| [`v256/`](v256) | 256-bit SIMD (AVX2): two blocks processed in parallel |
| [`cross-enc-dec/`](cross-enc-dec) | Encrypts with one implementation, decrypts with the other two |
| [`common/`](common) | Shared header, test-vector runner and benchmark scripts |
| [`results/`](results) | Benchmark spreadsheets and CSVs for each implementation |

Each folder builds on its own (`make -C v128`). Its README explains the design and lists its command-line options.

## Quick start

```bash
make            # build every implementation
make test       # RFC 8439 test vectors for each one + cross enc/dec test
make bench V=v256   # clock-cycle benchmark for one implementation
make clean
```

Requires `gcc` and an x86-64 CPU (`v256` needs AVX2). On Windows, use MinGW (`mingw32-make`).

## CI

[`.github/workflows/ci.yml`](.github/workflows/ci.yml) runs the tests on Linux for every push and pull request. On pushes to `main` it then runs the Windows clock-cycle benchmark for each implementation in parallel and uploads `results-<impl>` CSV artifacts.

## History

The 128-bit, 256-bit and cross enc/dec versions used to live in their own repos (`chacha20-128-vec`, `chacha20-256-vec`, `chacha20-cross-enc-dec`). They are now merged into this one, and the separate repos have been retired. They were imported with `git subtree`, so every original commit is kept. The older commits still use the original file paths (from before the move into subfolders), so use `git log --graph` to browse them rather than filtering by folder.
