#!/bin/bash
# Clock-cycle benchmark (ECRYPT-style): runs each of the 5 benchmark inputs
# RUNS times and writes "Test,Run,Clock Cycles" rows to a CSV.
#
# Usage: bench/clock_cycles_tests.sh <binary> [csv_file] [runs]
#   e.g. bench/clock_cycles_tests.sh build/chacha20-v256 results/v256.csv 100
set -eu

bin="${1:?usage: $0 <binary> [csv_file] [runs]}"
csv_file="${2:-generated_results.csv}"
runs="${3:-100}"

echo "Test,Run,Clock Cycles" > "$csv_file"
for test in 1 2 3 4 5; do
    for ((i = 1; i <= runs; i++)); do
        cycles=$("$bin" --clock-ct "$test" | awk '/Clock cycles per byte/ {print $5}')
        echo "$test,$i,$cycles" >> "$csv_file"
    done
    echo "Test $test: $runs runs done"
done
echo "Results written to $csv_file"
