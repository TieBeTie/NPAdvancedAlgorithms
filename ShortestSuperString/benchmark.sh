#!/bin/bash
set -e

SRC="shortest_superstring.cpp"
BIN="shortest_superstring"
INPUT="input.txt"
RESULTS_DIR="results"

echo "Compiling..."
g++ -std=c++17 -O3 -march=native -o "$BIN" "$SRC"

mkdir -p "$RESULTS_DIR"

run_algo() {
    local algo=$1
    local out="$RESULTS_DIR/output_${algo}.txt"
    echo "Running $algo..."
    ./"$BIN" "$algo" "$INPUT" "$out"
    echo "Output saved to $out"
}

run_algo lookahead
run_algo bidirectional
run_algo hybrid

echo "All tests done."
