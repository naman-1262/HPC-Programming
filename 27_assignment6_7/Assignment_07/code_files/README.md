# Assignment 07: Parallel Interpolation with Particle Mover (OpenMP)

## Project Structure

| File | Purpose |
|------|---------|
| `main.cpp` | Main pipeline; prints a `RESULT,...` line for the benchmark script |
| `utils.cpp` | Parallel PIC operations (interpolation, normalization, mover, denormalization) |
| `utils.h` | Header for utils |
| `init.cpp` / `init.h` | Point initialization & I/O |
| `input_file_maker.cpp` | Generates binary input files |
| `run_benchmark.sh` | **One-shot benchmark runner** – compiles, runs all 5 configs, generates CSV |
| `plot_results.py` | Reads CSV and generates speedup / execution-time plots into `plots/` |

---

## Quick Start (One Command)

```bash
bash run_benchmark.sh
```

This will:
1. Compile the input-file maker, serial binary, and parallel (OpenMP) binary
2. Generate input files for all 5 assignment configurations
3. Run serial + 2/4/8/16-thread benchmarks for each config
4. Save timing results to `benchmark_results.csv`
5. Call `plot_results.py` → save graphs to `plots/`

---

## Manual Steps

### Compile

```bash
# Serial
g++ main.cpp utils.cpp init.cpp -lm -O2 -o main_serial.out

# Parallel (OpenMP)
g++ main.cpp utils.cpp init.cpp -lm -O2 -fopenmp -o main_parallel.out
```

### Generate Input File

```bash
g++ input_file_maker.cpp -o input_maker.out
./input_maker.out          # follow the prompts
```

### Run

```bash
# Serial
./main_serial.out input.bin

# Parallel (e.g. 8 threads)
OMP_NUM_THREADS=8 ./main_parallel.out input.bin
```

### Plot Only (after benchmarks have been run)

```bash
python3 plot_results.py
```

---

## Correctness Check

```bash
./main_serial.out Test_input.bin
diff Mesh.out Test_Mesh.out    # should produce no output
```

---

## Parallelization Strategy

| Phase | Strategy | Race condition handling |
|-------|----------|------------------------|
| Interpolation | Thread-private mesh copies + parallel reduction | Eliminates all race conditions without atomics |
| Normalization | `reduction(min:)` / `reduction(max:)` + parallel map | OpenMP built-in reductions |
| Mover | Simple `parallel for` | No races (each particle is independent) |
| Denormalization | Simple `parallel for` | No races |

---

## Output Plots (in `plots/`)

| File | Contents |
|------|----------|
| `all_configs_combined.png` | 2×5 grid: time & speedup for all configs |
| `config1_*.png` … `config5_*.png` | Individual config plots |
| `overview_all_configs.png` | All configs overlaid on single axes |
