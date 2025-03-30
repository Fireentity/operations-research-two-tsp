#!/bin/bash
# This script processes three CSV files (results-VNS.csv, results-TS.csv, results-NN.csv)
# It computes the average cost for each parameter combination (for VNS and TS) and then
# selects the combination with the lowest average cost (minimization problem).
# For NN, which has no parameters, it simply finds the run with the lowest solution.
#
# The output average CSV files now include header columns.

# Check for required CSV files
for file in results-VNS.csv results-TS.csv results-NN.csv; do
    if [ ! -f "$file" ]; then
        echo "Error: $file not found!"
        exit 1
    fi
done

##############################
# Process VNS: columns: KickRepetitions,Nopt,Seed,Solution
##############################
echo "Processing results-VNS.csv..."
# Write header to output CSV file
echo "KickRepetitions,Nopt,AverageSolution" > vns_avg.csv
# Group by KickRepetitions and Nopt, compute average Solution and append to file
awk -F, 'NR>1 {
    key = $1","$2;
    sum[key] += $4;
    count[key]++
}
END {
    for (k in sum) {
        split(k, arr, ",");
        printf "%s,%s,%.6f\n", arr[1], arr[2], sum[k]/count[k]
    }
}' results-VNS.csv >> vns_avg.csv

# Find the parameter combination with the lowest average solution (ignoring header)
best_vns=$(tail -n +2 vns_avg.csv | sort -t, -k3n | head -n 1)
echo "Best VNS combination (KickRepetitions, Nopt, AverageSolution):"
echo "$best_vns"
echo ""

##############################
# Process TS: columns: Tenure,MaxStagnation,Seed,Solution
##############################
echo "Processing results-TS.csv..."
# Write header to output CSV file
echo "Tenure,MaxStagnation,AverageSolution" > ts_avg.csv
# Group by Tenure and MaxStagnation, compute average Solution and append to file
awk -F, 'NR>1 {
    key = $1","$2;
    sum[key] += $4;
    count[key]++
}
END {
    for (k in sum) {
        split(k, arr, ",");
        printf "%s,%s,%.6f\n", arr[1], arr[2], sum[k]/count[k]
    }
}' results-TS.csv >> ts_avg.csv

# Find the parameter combination with the lowest average solution (ignoring header)
best_ts=$(tail -n +2 ts_avg.csv | sort -t, -k3n | head -n 1)
echo "Best TS combination (Tenure, MaxStagnation, AverageSolution):"
echo "$best_ts"
echo ""

##############################
# Process NN: columns: Seed,Solution
##############################
echo "Processing results-NN.csv..."
# For NN, just find the best (lowest) solution among all seeds
best_nn=$(awk -F, 'NR>1 {
    if(min=="" || $2 < min) { min = $2 }
}
END {
    print min
}' results-NN.csv)
echo "Best NN solution (lowest Solution):"
echo "$best_nn"
