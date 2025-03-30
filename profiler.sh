#!/bin/bash
# This script profiles tsp_solver for three algorithms (NN, VNS, TS)
# with fixed parameters and separate CSV files for each algorithm.
#
# The standard fixed command-line parameters are:
#   --square-side 500
#   --x-square 2
#   --y-square 2
#   --seconds 30
#   --nodes 1500
#
# For every run, the seed is varied from 100 to 110.
#
# The experiments are organized in three main loops:
# 1) Nearest-neighbor (NN): only --nearest-neighbor is used.
#    Output file: results-NN.csv (columns: Seed,Solution)
# 2) VNS: uses --vns with its child parameters --kick-repetitions (1..10)
#    and --n-opt (3..11). Output file: results-VNS.csv
#    (columns: KickRepetitions,Nopt,Seed,Solution)
# 3) Tabu Search (TS): uses --tabu-search with its child parameters --tenure and --max-stagnation.
#    In this example, --tenure varies over (70,75,80,85,90) and --max-stagnation varies over (900,1000,1100).
#    Output file: results-TS.csv (columns: Tenure,MaxStagnation,Seed,Solution)

# Fixed parameters common to all runs.
fixed="./build/tsp_solver/tsp_solver --square-side 500 --x-square 2 --y-square 2 --seconds 30 --nodes 1500"

#####################################
# 1. Nearest-neighbor experiments (NN)
#####################################
NN_FILE="results-NN.csv"
echo "Seed,Solution" > "$NN_FILE"
echo "Running Nearest-neighbor experiments..."
for seed in {100..110}; do
    cmd="$fixed --nearest-neighbor --seed $seed"
    # Execute the command and capture its output.
    output=$($cmd)
    # Extract solution value; assuming output contains: "NN solution: <value>"
    solution=$(echo "$output" | grep "Nearest-neighbor solution" | awk '{print $3}')
    # Append the result to NN CSV.
    echo "$seed,$solution" >> "$NN_FILE"
done

#####################################
# 2. VNS experiments (VNS)
#####################################
VNS_FILE="results-VNS.csv"
echo "KickRepetitions,Nopt,Seed,Solution" > "$VNS_FILE"
echo "Running VNS experiments..."
# Loop over VNS parameters: --kick-repetitions (1 to 10) and --n-opt (3 to 11)
for kick in {1..10}; do
    for nopt in {3..11}; do
        for seed in {100..110}; do
            cmd="$fixed --vns --kick-repetitions $kick --n-opt $nopt --seed $seed"
            output=$($cmd)
            # Extract solution value; assuming output contains: "VNS solution: <value>"
            solution=$(echo "$output" | grep "VNS solution" | awk '{print $3}')
            echo "$kick,$nopt,$seed,$solution" >> "$VNS_FILE"
        done
    done
done

#####################################
# 3. Tabu Search experiments (TS)
#####################################
TS_FILE="results-TS.csv"
echo "Tenure,MaxStagnation,Seed,Solution" > "$TS_FILE"
echo "Running Tabu Search experiments..."
# Define Tabu Search parameter ranges.
tenure_values=(70 75 80 85 90)
max_stag_values=(900 1000 1100)
for tenure in "${tenure_values[@]}"; do
    for maxstag in "${max_stag_values[@]}"; do
        for seed in {100..110}; do
            cmd="$fixed --tabu-search --tenure $tenure --max-stagnation $maxstag --seed $seed"
            output=$($cmd)
            # Extract solution value; assuming output contains: "TS solution: <value>"
            solution=$(echo "$output" | grep "TS solution" | awk '{print $3}')
            echo "$tenure,$maxstag,$seed,$solution" >> "$TS_FILE"
        done
    done
done

echo "Experiments completed. Results saved in:"
echo "  Nearest-neighbor: $NN_FILE"
echo "  VNS: $VNS_FILE"
echo "  Tabu Search: $TS_FILE"
