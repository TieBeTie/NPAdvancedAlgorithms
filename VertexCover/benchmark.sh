set -e

SRC="VertexCover/vertex_cover.cpp"
BIN="VertexCover/vertex_cover"
INPUT="VertexCover/input.txt"
RESULTS_DIR="VertexCover/results"

echo "Compiling..."
g++ -std=c++17 -O2 -o "$BIN" "$SRC"

mkdir -p "$RESULTS_DIR"

run_algo() {
    local algo=$1
    local out="$RESULTS_DIR/output_${algo}.txt"
    echo "Running $algo..."
    "$BIN" "$algo" "$INPUT" "$out"
    echo "Output saved to $out"
}


run_algo greedy
run_algo matching_fast
run_algo matching_post

echo "All tests done."
