<h1 align="center">ChaCha20 Crossed-End Encryption</h1>


- This program utilizes the implementation of the regular [chacha20](../scalar) algorithm and its vectorized versions ([128](../v128) and [256](../v256)-bit respectively) to encrypt and decrypt a plaintext.

- For a given plaintext, each chacha20 implementation encrypts, and the other two decrypt.

- This demonstrates the correct functioning of the cryptosystem across its different vectorized and non-vectorized implementations.
- chacha20-crossed-enc-dec

---

## Tests

It builds directly from the sources in `../scalar`, `../v128` and `../v256`, so it always tests the current code of each implementation.

```bash
make            # build
make test       # run all three modes
./chacha20-crossed-enc-dec                  # default: scalar encrypts, vector versions decrypt
./chacha20-crossed-enc-dec --enc-chacha128  # 128-bit encrypts, the others decrypt
./chacha20-crossed-enc-dec --enc-chacha256  # 256-bit encrypts, the others decrypt
```
