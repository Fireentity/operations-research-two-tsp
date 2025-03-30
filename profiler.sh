#!/bin/bash
# This script profiles tsp_solver for three algorithms (NN, VNS, TS)
# with fixed parameters and separate CSV files for each algorithm.
#
# The fixed command-line parameters are:
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
#    In this example, --tenure varies over (70,75,80,85,90) and --max-stagnation over (900,1000,1100).
#    Output file: results-TS.csv (columns: Tenure,MaxStagnation,Seed,Solution)
#
# Each main experiment loop is run in a separate background process (thread).

# Fixed parameters common to all runs.
fixed="./build/tsp_solver/tsp_solver --square-side 500 --x-square 2 --y-square 2 --seconds 5 --nodes 500"
seeds=(100 105 110)
#####################################
# Function for Nearest-neighbor experiments (NN)
#####################################
run_nn () {
    NN_FILE="results-NN.csv"
    echo "Seed,Solution" > "$NN_FILE"
    echo "Running Nearest-neighbor experiments..."
    for seed in "${seeds[@]}"; do
        cmd="$fixed --nearest-neighbor --seed $seed"
        output=$($cmd)
        solution=$(echo "$output" | grep "NN solution" | awk '{print $3}')
        echo "$seed,$solution" >> "$NN_FILE"
    done
    echo "NN experiments completed."
}

#####################################
# Function for VNS experiments (VNS)
#####################################
run_vns () {
    VNS_FILE="results-VNS.csv"
    echo "KickRepetitions,Nopt,Seed,Solution" > "$VNS_FILE"
    echo "Running VNS experiments..."
    for kick in {1..10}; do
        for nopt in {3..11}; do
            for seed in "${seeds[@]}"; do
                cmd="$fixed --vns --kick-repetitions $kick --n-opt $nopt --seed $seed"
                output=$($cmd)
                solution=$(echo "$output" | grep "VNS solution" | awk '{print $3}')
                echo "$kick,$nopt,$seed,$solution" >> "$VNS_FILE"
            done
        done
    done
    echo "VNS experiments completed."
}

#####################################
# Function for Tabu Search experiments (TS)
#####################################
run_ts () {
    TS_FILE="results-TS.csv"
    echo "Tenure,MaxStagnation,Seed,Solution" > "$TS_FILE"
    echo "Running Tabu Search experiments..."
    tenure_values=(70 75 80 85 90)
    max_stag_values=(900 1000 1100)
    for tenure in "${tenure_values[@]}"; do
        for maxstag in "${max_stag_values[@]}"; do
            for seed in "${seeds[@]}"; do
                cmd="$fixed --tabu-search --tenure $tenure --max-stagnation $maxstag --seed $seed"
                output=$($cmd)
                solution=$(echo "$output" | grep "TS solution" | awk '{print $3}')
                echo "$tenure,$maxstag,$seed,$solution" >> "$TS_FILE"
            done
        done
    done
    echo "TS experiments completed."
}

# Run the three experiment functions concurrently.
run_nn &
PID_NN=$!

run_vns &
PID_VNS=$!

run_ts &
PID_TS=$!

# Wait for all background processes to finish.
wait $PID_NN $PID_VNS $PID_TS

echo "All experiments completed. Results saved in:"
echo "  Nearest-neighbor: results-NN.csv"
echo "  VNS: results-VNS.csv"
echo "  Tabu Search: results-TS.csv"
