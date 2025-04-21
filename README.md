# Traveling Salesman Problem (TSP) Solver
Operations Research 2 Project – University of Padua

> **TL;DR**: An optimization engine in **C**, orchestrated with **Bash** and **GNU Make/CMake**, implementing advanced heuristics (Nearest‑Neighbor, Tabu Search, Variable Neighborhood Search, GRASP) for the TSP. The code uses **pthread**, manual memory management, and **profiling**/debug scripts—all skills directly transferable to the **“Linux Kernel Bug Fixing Summer 2025”** project.

## Contents

1. [Motivation and Context](#motivation-and-context)
2. [Repository Architecture](#repository-architecture)
3. [Building (CMake → GNU Make/Ninja)](#building-cmake→gnumakeninja)
4. [Quick Run](#quick-run)
5. [Profiling & Benchmark](#profiling--benchmark)
6. [Testing & Continuous Integration](#testing--continuous-integration)
7. [Debugging and Code Quality](#debugging-and-code-quality)
8. [Alignment with “Linux Kernel Bug Fixing Summer 2025” Requirements](#alignment-with-linux-kernel-bug-fixing-summer-2025-requirements)
9. [Roadmap and Future Contributions](#roadmap-and-future-contributions)
10. [License](#license)

---

## Motivation and Context
This project started as a homework assignment for *Operations Research 2* and turns syllabus content (mathematical programming algorithms, modeling, combinatorial optimization) into code by applying it to a prototype problem: the Traveling Salesman Problem. The outcome is a modular solver in **C99**, accompanied by POSIX shell scripts for building, testing and measurement, suitable for *systems programming* scenarios typical of the Linux kernel.

## Repository Architecture
```
cli_lib/                 # Command‑line option parser           (C)
common/                  # Generic utilities, hashmap, timing  (C)
tsp_algo_lib/            # Algorithmic core and TSP structures  (C)
tsp_solver/              # Executable, algorithms, plotting    (C)
profiler/                # Bash scripts for experiments        (sh)
build.sh                 # Build helper                        (sh)
CMakeLists.txt           # Root build definition               (cmake)
```

## Building (CMake → GNU Make/Ninja)
The project is managed with **CMake**, but can generate **GNU Makefiles** or use Ninja:

```bash
# Production build with GNU Makefiles
cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=RelWithDebInfo -B build
make -C build -j$(nproc)

# Debug build with sanitizers
cmake -G Ninja -DCMAKE_BUILD_TYPE=Debug -DENABLE_ASAN=ON -B build_dbg
ninja -C build_dbg
```

> **Note:** The `build.sh` wrapper automates the steps above.

### Dependencies
* GCC ≥ 12 / Clang ≥ 16
* CMake ≥ 3.25 (generates Makefile or Ninja)
* GNU Make (if using the “Unix Makefiles” generator)
* Bash ≥ 5, *coreutils*
* gnuplot (for plots)

## Quick Run
```bash
# Random instance with 1000 nodes, 60s time limit, Nearest‑Neighbor heuristic
./build/tsp_solver/tsp_solver \
    --nodes 1000 --seconds 60 --nearest-neighbor
```

For the full list of options:
```bash
./tsp_solver --help
```

Available algorithms:
- **Nearest Neighbor**
- **Variable Neighborhood Search (VNS)** – parameters `--kick-repetitions` & `--n-opt`
- **Tabu Search** – parameters `--tenure` & `--max-stagnation`
- **GRASP** (greedy‑randomized)

## Profiling & Benchmark
The `profiler/` directory contains two scripts:

* `profiler.sh` – runs experiments in parallel (NN, VNS, TS) over a range of parameters
* `analyzer.sh` – analyzes CSVs and produces averages/plots

Output:
```
profiler/
├── results-*.csv       # raw data
├── *_avg.csv           # averages per configuration
└── *-plot.png          # cost vs. time graphs
```

## Testing & Continuous Integration
Unit tests written in C (using `<assert.h>`) reside in `*/tests/`.  
Run them with CTest:

```bash
cd build && ctest --output-on-failure
```

Suggested CI pipeline (GitHub Actions):

```yaml
- uses: actions/checkout@v4
- uses: rnorth/try-actions/setup-cmake@v1
- run: cmake -G Ninja -B build -DCMAKE_BUILD_TYPE=Debug
- run: ninja -C build
- run: ctest --test-dir build --output-on-failure
```

## License
This project is released under the **MIT** license (see `LICENSE`).

*Author: Lorenzo Croce and Alberto Bottari* – April 2025