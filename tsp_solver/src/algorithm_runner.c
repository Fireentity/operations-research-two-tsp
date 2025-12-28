#include "algorithm_runner.h"
#include "nearest_neighbor.h"
#include "variable_neighborhood_search.h"
#include "tabu_search.h"
#include "grasp.h"
#include "../include/plot_util.h"
#include "c_util.h"
#include "logger.h"
#include "constants.h"

#include <stdio.h>
#include <linux/limits.h>
#include <string.h>
#include <strings.h> // for strcasecmp
#include <stdlib.h>

#include "benders_loop.h"
#include "branch_and_cut.h"
#include "extra_mileage.h"
#include "genetic.h"
#include "hard_fixing.h"

static void execute_and_report(const TspAlgorithm *algo,
                               const TspInstance *instance,
                               const char *plot_file,
                               const char *costs_file) {
    // --- Cost Recorder ---
    CostRecorder *recorder = cost_recorder_create(RECORDER_INITIAL_CAPACITY);

    // --- Solution ---
    TspSolution *solution = tsp_solution_create(instance);

    // --- Run Algorithm ---
    tsp_algorithm_run(algo, instance, solution, recorder);

    // --- Extract Tour ---
    const int n = tsp_instance_get_num_nodes(instance);
    int *tour_buffer = malloc((n + 1) * sizeof(int));
    check_alloc(tour_buffer);

    tsp_solution_get_tour(solution, tour_buffer);
    const double cost = tsp_solution_get_cost(solution);

    // --- Plotting ---
    plot_tour(tour_buffer, n, tsp_instance_get_nodes(instance), plot_file);
    plot_costs_evolution(cost_recorder_get_costs(recorder), cost_recorder_get_count(recorder), costs_file);

    if_verbose(VERBOSE_INFO, "%s solution: %lf\n", algo->name, cost);

    // --- Cleanup ---
    free(tour_buffer);
    cost_recorder_destroy(recorder);
    tsp_solution_destroy(solution);
    tsp_algorithm_destroy((TspAlgorithm *) algo);
}

// Helper to parse CLI string to Library Enum
static HFHeuristicType parse_hf_heuristic(const char *name) {
    if (!name) return HF_HEURISTIC_VNS;
    if (strcasecmp(name, "nn") == 0) return HF_HEURISTIC_NN;
    if (strcasecmp(name, "em") == 0 || strcasecmp(name, "extra_mileage") == 0) return HF_HEURISTIC_EXTRA_MILEAGE;
    if (strcasecmp(name, "tabu") == 0) return HF_HEURISTIC_TABU;
    if (strcasecmp(name, "grasp") == 0) return HF_HEURISTIC_GRASP;
    if (strcasecmp(name, "vns") == 0) return HF_HEURISTIC_VNS;

    if_verbose(VERBOSE_INFO, "[Warning] Unknown heuristic '%s', defaulting to VNS.\n", name);
    return HF_HEURISTIC_VNS;
}

void run_selected_algorithms(const TspInstance *instance, const CmdOptions *options) {
    char full_plot_path[PATH_MAX];
    char full_costs_path[PATH_MAX];

    // Helper macro to construct paths based on --plot-path
#define BUILD_PATHS(plot_fname, cost_fname) \
        if (options->plots_path && strlen(options->plots_path) > 0) { \
            join_path(full_plot_path, options->plots_path, plot_fname, PATH_MAX); \
            join_path(full_costs_path, options->plots_path, cost_fname, PATH_MAX); \
        } else { \
            snprintf(full_plot_path, PATH_MAX, "%s", plot_fname); \
            snprintf(full_costs_path, PATH_MAX, "%s", cost_fname); \
        }

    // --- NEAREST NEIGHBOR ---
    if (options->nn_params.enable) {
        NNConfig cfg = {
            .time_limit = options->nn_params.time_limit,
            .seed = options->inst.seed
        };
        TspAlgorithm algo = nn_create(cfg);
        BUILD_PATHS(options->nn_params.plot_file, options->nn_params.cost_file);
        execute_and_report(&algo, instance, full_plot_path, full_costs_path);
    }

    // --- VARIABLE NEIGHBORHOOD SEARCH ---
    if (options->vns_params.enable) {
        VNSConfig cfg = {
            .min_k = (int) options->vns_params.min_k,
            .max_k = (int) options->vns_params.max_k,
            .kick_repetition = (int) options->vns_params.kick_repetitions,
            .max_stagnation = (int) options->vns_params.max_stagnation,
            .time_limit = options->vns_params.time_limit,
            .seed = options->inst.seed
        };
        TspAlgorithm algo = vns_create(cfg);
        BUILD_PATHS(options->vns_params.plot_file, options->vns_params.cost_file);
        execute_and_report(&algo, instance, full_plot_path, full_costs_path);
    }

    // --- TABU SEARCH ---
    if (options->tabu_params.enable) {
        TabuConfig cfg = {
            .min_tenure = (int) options->tabu_params.min_tenure,
            .max_tenure = (int) options->tabu_params.max_tenure,
            .max_stagnation = (int) options->tabu_params.max_stagnation,
            .time_limit = options->tabu_params.time_limit,
            .seed = options->inst.seed
        };
        TspAlgorithm algo = tabu_create(cfg);
        BUILD_PATHS(options->tabu_params.plot_file, options->tabu_params.cost_file);
        execute_and_report(&algo, instance, full_plot_path, full_costs_path);
    }

    // --- GRASP ---
    if (options->grasp_params.enable) {
        GraspConfig cfg = {
            .rcl_size = (int) options->grasp_params.rcl_size,
            .probability = options->grasp_params.probability,
            .max_stagnation = (int) options->grasp_params.max_stagnation,
            .time_limit = options->grasp_params.time_limit,
            .seed = options->inst.seed
        };
        TspAlgorithm algo = grasp_create(cfg);
        BUILD_PATHS(options->grasp_params.plot_file, options->grasp_params.cost_file);
        execute_and_report(&algo, instance, full_plot_path, full_costs_path);
    }

    // --- EXTRA MILEAGE ---
    if (options->em_params.enable) {
        EMConfig cfg = {
            .time_limit = options->em_params.time_limit,
            .seed = options->inst.seed
        };
        TspAlgorithm algo = em_create(cfg);
        BUILD_PATHS(options->em_params.plot_file, options->em_params.cost_file);
        execute_and_report(&algo, instance, full_plot_path, full_costs_path);
    }

    // --- GENETIC ALGORITHM ---
    if (options->genetic_params.enable) {
        GeneticConfig cfg = {
            .time_limit = options->genetic_params.time_limit,
            .population_size = (int) options->genetic_params.population_size,
            .elite_count = (int) options->genetic_params.elite_count,
            .mutation_rate = options->genetic_params.mutation_rate,
            .crossover_cut_min_ratio = (int) options->genetic_params.crossover_cut_min_ratio,
            .crossover_cut_max_ratio = (int) options->genetic_params.crossover_cut_max_ratio,
            .tournament_size = options->genetic_params.tournament_size,
            .init_grasp_rcl_size = options->genetic_params.init_grasp_rcl_size,
            .init_grasp_prob = options->genetic_params.init_grasp_prob,
            .init_grasp_percent = options->genetic_params.init_grasp_percent,
            .seed = options->inst.seed
        };

        TspAlgorithm algo = genetic_create(cfg);
        BUILD_PATHS(options->genetic_params.plot_file, options->genetic_params.cost_file);
        execute_and_report(&algo, instance, full_plot_path, full_costs_path);
    }

    // --- BENDERS DECOMPOSITION ---
    if (options->benders_params.enable) {
        BendersConfig cfg = {
            .time_limit = options->benders_params.time_limit,
            .max_iterations = (int) options->benders_params.max_iterations
        };
        TspAlgorithm algo = benders_create(cfg);
        BUILD_PATHS(options->benders_params.plot_file, options->benders_params.cost_file);
        execute_and_report(&algo, instance, full_plot_path, full_costs_path);
    }

    // --- BRANCH AND CUT ---
    if (options->bc_params.enable) {
        BranchCutConfig cfg = {
            .time_limit = options->bc_params.time_limit,
            .num_threads = (int) options->bc_params.num_threads
        };
        TspAlgorithm algo = branch_and_cut_create(cfg);
        BUILD_PATHS(options->bc_params.plot_file, options->bc_params.cost_file);
        execute_and_report(&algo, instance, full_plot_path, full_costs_path);
    }

    // --- HARD FIXING (MATHEURISTIC) ---
    if (options->hf_params.enable) {
        HardFixingConfig cfg = {
            .time_limit = options->hf_params.time_limit,
            .fixing_rate = options->hf_params.fixing_rate,
            .heuristic_time_ratio = options->hf_params.heuristic_ratio,
            .heuristic_type = parse_hf_heuristic(options->hf_params.heuristic_name),
            .seed = options->inst.seed,
            .heuristic_args = NULL
        };

        // Construct the sub-configuration structure dynamically based on the chosen heuristic
        // This ensures flags like --vns-min-k are respected inside Hard Fixing
        switch (cfg.heuristic_type) {
            case HF_HEURISTIC_VNS: {
                VNSConfig *vns = malloc(sizeof(VNSConfig));
                check_alloc(vns);
                *vns = (VNSConfig){
                    .min_k = (int)options->vns_params.min_k,
                    .max_k = (int)options->vns_params.max_k,
                    .kick_repetition = (int)options->vns_params.kick_repetitions,
                    .max_stagnation = (int)options->vns_params.max_stagnation,
                    .seed = options->inst.seed
                    // time_limit is managed dynamically by hard_fixing logic
                };
                cfg.heuristic_args = vns;
                break;
            }
            case HF_HEURISTIC_TABU: {
                TabuConfig *tabu = malloc(sizeof(TabuConfig));
                check_alloc(tabu);
                *tabu = (TabuConfig){
                    .min_tenure = (int)options->tabu_params.min_tenure,
                    .max_tenure = (int)options->tabu_params.max_tenure,
                    .max_stagnation = (int)options->tabu_params.max_stagnation,
                    .seed = options->inst.seed
                };
                cfg.heuristic_args = tabu;
                break;
            }
            case HF_HEURISTIC_GRASP: {
                GraspConfig *grasp = malloc(sizeof(GraspConfig));
                check_alloc(grasp);
                *grasp = (GraspConfig){
                    .rcl_size = (int)options->grasp_params.rcl_size,
                    .probability = options->grasp_params.probability,
                    .max_stagnation = (int)options->grasp_params.max_stagnation,
                    .seed = options->inst.seed
                };
                cfg.heuristic_args = grasp;
                break;
            }
            default:
                // NN and EM do not require extra complex config, NULL triggers defaults in hard_fixing.c
                cfg.heuristic_args = NULL;
                break;
        }

        TspAlgorithm algo = hard_fixing_create(cfg);
        BUILD_PATHS(options->hf_params.plot_file, options->hf_params.cost_file);
        execute_and_report(&algo, instance, full_plot_path, full_costs_path);
    }

#undef BUILD_PATHS
}