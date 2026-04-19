#!/bin/bash
# =============================================================================
# run_benchmark.sh
#
# Builds the serial and parallel versions, generates all 5 input configs,
# runs each config with 1 (serial) and 2,4,8,16 threads, then calls
# plot_results.py to generate the speedup/time graphs automatically.
#
# Usage:  bash run_benchmark.sh
# =============================================================================

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

INPUT_MAKER="input_maker.out"
SERIAL_BIN="main_serial.out"
PARALLEL_BIN="main_parallel.out"
RESULTS_CSV="benchmark_results.csv"

echo "============================================================"
echo "  HPC Assignment 07 – Benchmark Runner"
echo "============================================================"

# ── 1. Compile input-file maker ──────────────────────────────────
echo "[1/4] Compiling input_file_maker.cpp ..."
g++ input_file_maker.cpp -o $INPUT_MAKER
echo "      OK"

# ── 2. Compile serial version (no OpenMP) ────────────────────────
echo "[2/4] Compiling serial binary ..."
g++ main.cpp utils.cpp init.cpp -lm -O2 -o $SERIAL_BIN
echo "      OK"

# ── 3. Compile parallel version (with OpenMP) ────────────────────
echo "[3/4] Compiling parallel binary ..."
g++ main.cpp utils.cpp init.cpp -lm -O2 -fopenmp -D_OPENMP -o $PARALLEL_BIN
echo "      OK"

# ── 4. Define the 5 benchmark configurations ─────────────────────
# Format: "NX NY POINTS MAXITER LABEL"
declare -a CONFIGS=(
    "250 100 900000 10  Config1_250x100_0.9M"
    "250 100 5000000 10 Config2_250x100_5M"
    "500 200 3600000 10 Config3_500x200_3.6M"
    "500 200 20000000 10 Config4_500x200_20M"
    "1000 400 14000000 10 Config5_1000x400_14M"
)

THREAD_COUNTS=(2 4 8 16)

# Write CSV header
echo "config,nx,ny,points,maxiter,threads,time_sec" > $RESULTS_CSV

echo "[4/4] Running benchmarks ..."
echo ""

config_idx=0
for cfg in "${CONFIGS[@]}"; do
    read -r NX NY POINTS MAXITER LABEL <<< "$cfg"
    config_idx=$((config_idx + 1))

    INPUT_FILE="input_cfg${config_idx}.bin"

    echo "------------------------------------------------------------"
    echo "  Config $config_idx : NX=$NX NY=$NY Points=$POINTS Maxiter=$MAXITER"
    echo "------------------------------------------------------------"

    # Generate input file using a here-doc into the maker
    echo "  Generating input file $INPUT_FILE ..."
    printf "%d %d\n%d\n%d\n" $NX $NY $POINTS $MAXITER | ./$INPUT_MAKER > /dev/null
    mv input.bin $INPUT_FILE
    echo "  Generated."

    # ── Serial run (1 thread) ──────────────────────────────────
    echo "  Running serial (1 thread) ..."
    OUTPUT=$(./$SERIAL_BIN $INPUT_FILE 2>/dev/null)
    RESULT_LINE=$(echo "$OUTPUT" | grep "^RESULT,")
    if [ -n "$RESULT_LINE" ]; then
        IFS=',' read -r _ rnx rny rpts riter rthreads rtime <<< "$RESULT_LINE"
        echo "    Time = ${rtime}s"
        echo "$LABEL,$NX,$NY,$POINTS,$MAXITER,1,$rtime" >> $RESULTS_CSV
    else
        echo "    WARNING: could not parse serial result"
        echo "$LABEL,$NX,$NY,$POINTS,$MAXITER,1,0" >> $RESULTS_CSV
    fi

    # ── Parallel runs ─────────────────────────────────────────
    for T in "${THREAD_COUNTS[@]}"; do
        echo "  Running parallel with $T threads ..."
        export OMP_NUM_THREADS=$T
        OUTPUT=$(OMP_NUM_THREADS=$T ./$PARALLEL_BIN $INPUT_FILE 2>/dev/null)
        RESULT_LINE=$(echo "$OUTPUT" | grep "^RESULT,")
        if [ -n "$RESULT_LINE" ]; then
            IFS=',' read -r _ rnx rny rpts riter rthreads rtime <<< "$RESULT_LINE"
            echo "    Time = ${rtime}s"
            echo "$LABEL,$NX,$NY,$POINTS,$MAXITER,$T,$rtime" >> $RESULTS_CSV
        else
            echo "    WARNING: could not parse result for $T threads"
            echo "$LABEL,$NX,$NY,$POINTS,$MAXITER,$T,0" >> $RESULTS_CSV
        fi
    done

    echo ""
done

echo "============================================================"
echo "  All benchmarks complete. Results saved to: $RESULTS_CSV"
echo "============================================================"
echo ""

# ── 5. Plot results ───────────────────────────────────────────────
if command -v python3 &>/dev/null; then
    echo "Generating plots with plot_results.py ..."
    python3 plot_results.py
else
    echo "python3 not found – skipping plot generation."
    echo "Run 'python3 plot_results.py' manually to generate graphs."
fi
