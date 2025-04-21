# Traveling Salesman Problem (TSP) Solver
Progetto di Ricerca Operativa 2 – Università degli Studi di Padova

> **TL;DR**: motore di ottimizzazione in **C**, orchestrato da **Bash** e **GNU Make/CMake**, che implementa euristiche avanzate (Nearest‑Neighbor, Tabu Search, Variable Neighborhood Search, GRASP) per il TSP. Il codice usa **pthread**, gestione manuale della memoria e script di **profiling**/debug—tutte competenze direttamente trasferibili al progetto **“Linux Kernel Bug Fixing Summer 2025”**.

---

## Contenuti

1. [Motivazioni e inquadramento](#motivazioni-e-inquadramento)
2. [Architettura del repository](#architettura-del-repository)
3. [Costruzione (CMake→GNUMake∙Ninja)](#costruzione)
4. [Esecuzione rapida](#esecuzione-rapida)
5. [Profiling & Benchmark](#profiling--benchmark)
6. [Testing & Continuous Integration](#testing--continuous-integration)
7. [Debug e qualità del codice](#debug-e-qualità-del-codice)
8. [Aderenza ai requisiti “Linux Kernel Bug Fixing Summer 2025”](#aderenza-ai-requisiti-linux-kernel-bug-fixing-summer-2025)
9. [Road‑map e contributi futuri](#road-map-e-contributi-futuri)
10. [Licenza](#licenza)

---

## Motivazioni e inquadramento
Questo progetto nasce come homework di *Operations Research2* e traduce in codice i contenuti del syllabus (algoritmi di programmazione matematica, modellazione, ottimizzazione combinatoria) applicandoli ad un prototipo di problema: il TravelingSalesmanProblem. Il risultato è un solver modulare in linguaggio **C99**, corredato di script POSIX shell per build, test e misurazioni, adatto a scenari di *systems programming* tipici del kernel Linux.

---

## Architettura del repository
```
.
├── cli_lib/                 # parser di opzioni da linea di comando  (C)
├── common/                  # utilità generiche, hashmap, chrono     (C)
├── tsp_algo_lib/            # core algoritmico e strutture TSP       (C)
├── tsp_solver/              # executable, algoritmi, plotting        (C)
├── profiler/                # script Bash per esperimenti            (sh)
├── build.sh                 # build helper                           (sh)
└── CMakeLists.txt           # root build definition                  (cmake)
```
*Esempio di header C*: strutture dati per l’istanza TSP   
*Esempio di script Bash*: orchestrazione esperimenti con profiler.sh

---

## Costruzione
Il progetto è gestito da **CMake**, ma può generare **Makefile GNU** oppure buildNinja:

```bash
# Build di produzione con Makefile GNU
cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=RelWithDebInfo -B build
make -C build -j$(nproc)

# Build di debug con sanitizzatori
cmake -G Ninja -DCMAKE_BUILD_TYPE=Debug -DENABLE_ASAN=ON -B build_dbg
ninja -C build_dbg
```

> **Nota:** il wrapper `build.sh` automatizza i passi sopra

### Dipendenze
* GCC≥12 / Clang≥16
* CMake≥3.25 (genera Makefile o Ninja)
* GNUMake (se si usa il generator “UnixMakefiles”)
* Bash ≥5, *coreutils*
* gnuplot (per i grafici)

---

## Esecuzione rapida
```bash
# Istanza casuale da 1000 nodi, 60s time‑limit, euristica Nearest‑Neighbor
./build/tsp_solver/tsp_solver \
    --nodes 1000 --seconds 60 --nearest-neighbor
```

Per la lista completa di opzioni:
```bash
./tsp_solver --help
```

Algoritmi disponibili:
- **Nearest Neighbor**
- **Variable Neighborhood Search (VNS)** ‑ parametri `--kick-repetitions` & `--n-opt`
- **Tabu Search** ‑ parametri `--tenure` & `--max-stagnation`
- **GRASP** (greedy‑randomised)

---

## Profiling & Benchmark
La cartella `profiler/` contiene due script:

* `profiler.sh` – lancia esperimenti in parallelo (NN,VNS,TS) su range di parametri
* `analyzer.sh` – analizza CSV e produce medie/plot

Output:
```
profiler/
├── results-*.csv       # raw data
├── *_avg.csv           # medie per configurazione
└── *-plot.png          # grafici costi vs tempo
```

---

## Testing & Continuous Integration
Unit‑test scritti in C (basati su `<assert.h>`) risiedono in `*/tests/`.  
Esecuzione con CTest:

```bash
cd build && ctest --output-on-failure
```

Pipeline CI suggerita (GitHub Actions):

```yaml
- uses: actions/checkout@v4
- uses: rnorth/try-actions/setup-cmake@v1
- run: cmake -G Ninja -B build -DCMAKE_BUILD_TYPE=Debug
- run: ninja -C build
- run: ctest --test-dir build --output-on-failure
```

---

## Licenza
Questo progetto è rilasciato sotto licenza **MIT** (vedi `LICENSE`).

---

*Autore: Lorenzo Croce and Alberto Bottari* – aprile 2025