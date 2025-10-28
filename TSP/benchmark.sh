#!/bin/bash
set -e

BIN="./tsp"
INPUT="input.txt"
RESULTS_DIR="results"

ALGORITHMS=(
  2approx
  15approx_improved
  nn
  nn_2opt
)

echo "Building..."
make clean >/dev/null
make >/dev/null

if [ ! -f "$BIN" ]; then
  echo "Build failed"
  exit 1
fi

mkdir -p "$RESULTS_DIR"

run_algo() {
  local algo=$1
  local out="$RESULTS_DIR/output_${algo}.txt"

  echo "Running $algo..."
  "$BIN" "$algo" "$INPUT" "$out"
  echo "Output saved to $out"
}

for algo in "${ALGORITHMS[@]}"; do
  run_algo "$algo"
done

echo "All tests done."
