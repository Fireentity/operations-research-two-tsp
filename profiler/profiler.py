#!/usr/bin/env python3
import json
import subprocess
import csv
import re
import os
import glob
import time
import argparse
import itertools
from datetime import datetime
#TODO maybe optuna? idk
# --- CONFIGURATION ---
SOLVER_PATH = "../build/tsp_solver/tsp_solver"
RESULTS_DIR = "results"
TSPLIB_DIR = "../TSPLIB95"

# Regex to capture "Algorithm Name solution: 12345.67"
COST_PATTERN = re.compile(r"([^\r\n]+) solution:\s+([\d\.]+)")

def run_solver(instance_path, flag_list, seed, time_limit):
    """Executes the C solver with the given flags."""
    cmd = [
              SOLVER_PATH,
              "--mode", "1", # 1 = FILE mode
              "--file", instance_path,
              "--seed", str(seed)
          ] + flag_list

    start_time = time.time()
    try:
        # Run with safety timeout (+10s)
        result = subprocess.run(
            cmd, capture_output=True, text=True, timeout=time_limit + 10
        )
        duration = time.time() - start_time

        if result.returncode != 0:
            # print(f"[Error {result.returncode}] {instance_path}") # Debug if needed
            return None

        matches = COST_PATTERN.findall(result.stdout)
        # Returns list of (AlgoName, Cost, Duration)
        return [(m[0].strip(), float(m[1]), duration) for m in matches]

    except subprocess.TimeoutExpired:
        print(f"  [Timeout] {os.path.basename(instance_path)}")
        return None
    except FileNotFoundError:
        print(f"  [Error] Solver not found at {SOLVER_PATH}")
        return None

def is_valid_combination(params):
    """Filters out illogical parameter combinations (e.g. min > max)."""
    # VNS Check
    if "--vns-min-k" in params and "--vns-max-k" in params:
        if int(params["--vns-min-k"]) > int(params["--vns-max-k"]):
            return False
    # Tabu Check
    if "--ts-min-tenure" in params and "--ts-max-tenure" in params:
        if int(params["--ts-min-tenure"]) > int(params["--ts-max-tenure"]):
            return False
    return True

def generate_grid_experiments(grid_config):
    """Generates the Cartesian product of parameters."""
    base_flags = grid_config.get("base_flags", "").split()
    prefix = grid_config.get("algo_prefix", "")

    # Extract parameter lists
    param_names = list(grid_config["params"].keys())
    param_values = list(grid_config["params"].values())

    experiments = []

    # itertools.product creates the Cartesian product
    for combination in itertools.product(*param_values):
        current_params = dict(zip(param_names, combination))

        if not is_valid_combination(current_params):
            continue

        # Build flag list and readable name
        flags = base_flags.copy()
        name_parts = []

        for flag, value in current_params.items():
            flags.extend([flag, str(value)])
            # Clean name: "--vns-min-k" -> "min-k_2"
            short_name = flag.lstrip("-").replace(f"{prefix}-", "")
            name_parts.append(f"{short_name}_{value}")

        experiments.append({
            "name": "|".join(name_parts),
            "flags_list": flags
        })

    return experiments

def main():
    if not os.path.exists(RESULTS_DIR):
        os.makedirs(RESULTS_DIR)

    parser = argparse.ArgumentParser()
    parser.add_argument("config_file", help="Path to JSON configuration file")
    args = parser.parse_args()

    try:
        with open(args.config_file, "r") as f:
            config = json.load(f)
            print(f"Loaded: {config.get('description', args.config_file)}")
    except Exception as e:
        print(f"Error loading JSON: {e}")
        return

    # Determine Experiment List
    experiment_queue = []
    if "grid" in config:
        print("-> Mode: GRID SEARCH (Cartesian Product)")
        experiment_queue = generate_grid_experiments(config["grid"])
    elif "experiments" in config:
        print("-> Mode: EXPLICIT LIST")
        for exp in config["experiments"]:
            exp["flags_list"] = exp["flags"].split()
            experiment_queue.append(exp)

    # Get Instances
    instances = []
    if config.get("instances") == "all":
        instances = glob.glob(os.path.join(TSPLIB_DIR, "*.tsp"))
    else:
        for name in config["instances"]:
            p = os.path.join(TSPLIB_DIR, name)
            if os.path.exists(p): instances.append(p)
    instances.sort()

    timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
    csv_file = os.path.join(RESULTS_DIR, f"results_{timestamp}.csv")

    total_ops = len(instances) * len(config.get("seeds", [1])) * len(experiment_queue)
    print(f"Total Runs: {total_ops}")

    with open(csv_file, "w", newline="") as f:
        writer = csv.writer(f)
        writer.writerow(["Instance", "Algorithm", "Config", "Cost", "Time", "Seed"])

        count = 0
        for inst in instances:
            inst_name = os.path.basename(inst)
            for exp in experiment_queue:
                # Heuristic to extract time limit for timeout safety
                t_limit = 60.0
                if "--seconds" in exp["flags_list"]: # generic
                    idx = exp["flags_list"].index("--seconds")
                    t_limit = float(exp["flags_list"][idx+1])
                else: # specific algo seconds (e.g., --vns-seconds)
                    for fl in exp["flags_list"]:
                        if fl.endswith("-seconds"):
                            idx = exp["flags_list"].index(fl)
                            t_limit = float(exp["flags_list"][idx+1])
                            break

                for seed in config.get("seeds", [42]):
                    count += 1
                    print(f"[{count}/{total_ops}] {inst_name} | {exp['name']}")

                    results = run_solver(inst, exp["flags_list"], seed, t_limit)

                    if results:
                        for (algo, cost, duration) in results:
                            writer.writerow([inst_name, algo, exp["name"], cost, duration, seed])
                            f.flush()

    print(f"\nDone. Results: {csv_file}")

if __name__ == "__main__":
    main()