#!/bin/bash
# Runs the 5 RFC 8439 encryption and decryption test vectors against one
# implementation and exits non-zero if any of them fails.
# Usage: common/run_test_vectors.sh <dir containing the chacha20 binary>
set -u
dir="${1:?usage: $0 <implementation dir>}"
fail=0
for n in 1 2 3 4 5; do
    for mode in --enc-tv --dec-tv; do
        if "$dir/chacha20" "$mode" "$n" | grep -q "successful"; then
            echo "  ok    $dir $mode $n"
        else
            echo "  FAIL  $dir $mode $n"; fail=1
        fi
    done
done
exit $fail
