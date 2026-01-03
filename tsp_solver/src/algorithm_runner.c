#include "algorithm_runner.h"
#include "nearest_neighbor.h"
#include "variable_neighborhood_search.h"
#include "tabu_search.h"
#include "grasp.h"
#include "plot_util.h"
#include "c_util.h"
#include "logger.h"
#include "constants.h"

#include <stdio.h>
#include <linux/limits.h>
#include <string.h>
#include <strings.h>
#include <pthread.h>

#include "benders_loop.h"
#include "branch_and_cut.h"
#include "extra_mileage.h"
#include "genetic.h"
#include "hard_fixing.h"
#include "local_branching.h"
#include "heuristic_types.h"

typedef struct {
    TspSolverFn run_fn;
    const TspInstance *instance;
    TspSolution *solution;
    void *local_config;

    void (*free_config)(void *);

    unsigned int thread_id;
} WorkerArgs;

static void *worker_thread_func(void *arg) {
    WorkerArgs *w = arg;
    w->run_fn(w->instance, w->solution, w->local_config, NULL);
    if (w->free_config && w->local_config) {
        w->free_config(w->local_config);
    }
    return NULL;
}

static void execute_parallel(const TspAlgorithm *algo,
                             const TspInstance *instance,
                             TspSolution *solution,
                             unsigned int num_threads) {
    pthread_t *threads = tsp_malloc(num_threads * sizeof(pthread_t));
    WorkerArgs *args = tsp_malloc(num_threads * sizeof(WorkerArgs));

    if_verbose(VERBOSE_INFO, ">>> Starting Parallel Execution: %s with %d threads\n", algo->name, num_threads);

    for (size_t i = 0; i < num_threads; i++) {
        args[i].run_fn = algo->run;
        args[i].instance = instance;
        args[i].solution = solution;
        args[i].free_config = algo->free_config;
        args[i].thread_id = i;
        args[i].local_config = algo->clone_config(algo->config, i);

        if (pthread_create(&threads[i], NULL, worker_thread_func, &args[i]) != 0) {
            fprintf(stderr, "Error creating thread %ld\n", i);
        }
    }

    for (size_t i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    tsp_free(threads);
    tsp_free(args);
}

static void execute_and_report(const TspAlgorithm *algo,
                               const TspInstance *instance,
                               const char *plot_file,
                               const char *costs_file,
                               unsigned int num_threads) {
    CostRecorder *recorder = cost_recorder_create(RECORDER_INITIAL_CAPACITY);
    TspSolution *solution = tsp_solution_create(instance);

    if (num_threads > 1 && algo->clone_config != NULL) {
        execute_parallel(algo, instance, solution, num_threads);
        cost_recorder_add(recorder, tsp_solution_get_cost(solution));
    } else {
        if (num_threads > 1) {
            if_verbose(VERBOSE_INFO, "[WARN] Algorithm %s does not support multi-threading. Running sequentially.\n",
                       algo->name);
        }
        tsp_algorithm_run(algo, instance, solution, recorder);
    }

    const int n = tsp_instance_get_num_nodes(instance);
    int *tour_buffer = tsp_malloc((n + 1) * sizeof(int));

    tsp_solution_get_tour(solution, tour_buffer);
    const double cost = tsp_solution_get_cost(solution);

    plot_tour(tour_buffer, n, tsp_instance_get_nodes(instance), plot_file);
    plot_costs_evolution(cost_recorder_get_costs(recorder), cost_recorder_get_count(recorder), costs_file);

    if_verbose(VERBOSE_INFO, "%s solution: %lf\n", algo->name, cost);

    tsp_free(tour_buffer);
    cost_recorder_destroy(recorder);
    tsp_solution_destroy(solution);
    tsp_algorithm_destroy((TspAlgorithm *) algo);
}

static HeuristicType parse_warm_start_heuristic(const char *name) {
    if (!name) return VNS;
    if (strcasecmp(name, "nn") == 0) return NN;
    if (strcasecmp(name, "em") == 0 || strcasecmp(name, "extra_mileage") == 0) return EM;
    if (strcasecmp(name, "tabu") == 0) return TABU;
    if (strcasecmp(name, "grasp") == 0) return GRASP;
    if (strcasecmp(name, "ga") == 0 || strcasecmp(name, "genetic") == 0) return GENETIC;
    if (strcasecmp(name, "vns") == 0) return VNS;

    if_verbose(VERBOSE_INFO, "[Warning] Unknown heuristic '%s', defaulting to VNS.\n", name);
    return VNS;
}

static void *create_heuristic_config(HeuristicType type, const CmdOptions *options) {
    switch (type) {
        case VNS: {
            VNSConfig *vns = tsp_malloc(sizeof(VNSConfig));
            *vns = (VNSConfig){
                .min_k = (int) options->vns_params.min_k,
                .max_k = (int) options->vns_params.max_k,
                .kick_repetition = (int) options->vns_params.kick_repetitions,
                .max_stagnation = (int) options->vns_params.max_stagnation,
                .seed = options->inst.seed
            };
            return vns;
        }
        case TABU: {
            TabuConfig *tabu = tsp_malloc(sizeof(TabuConfig));
            *tabu = (TabuConfig){
                .min_tenure = (int) options->tabu_params.min_tenure,
                .max_tenure = (int) options->tabu_params.max_tenure,
                .max_stagnation = (int) options->tabu_params.max_stagnation,
                .seed = options->inst.seed
            };
            return tabu;
        }
        case GRASP: {
            GraspConfig *grasp = tsp_malloc(sizeof(GraspConfig));
            *grasp = (GraspConfig){
                .rcl_size = (int) options->grasp_params.rcl_size,
                .probability = options->grasp_params.probability,
                .max_stagnation = (int) options->grasp_params.max_stagnation,
                .seed = options->inst.seed
            };
            return grasp;
        }
        case GENETIC: {
            GeneticConfig *ga = tsp_malloc(sizeof(GeneticConfig));
            *ga = (GeneticConfig){
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
            return ga;
        }
        case NN:
        case EM:
        default:
            return NULL;
    }
}

void run_selected_algorithms(const TspInstance *instance, const CmdOptions *options) {
    char full_plot_path[PATH_MAX];
    char full_costs_path[PATH_MAX];
    unsigned int threads = options->num_threads;

#define BUILD_PATHS(plot_fname, cost_fname) \
        if (options->plots_path && strlen(options->plots_path) > 0) { \
            join_path(full_plot_path, options->plots_path, plot_fname, PATH_MAX); \
            join_path(full_costs_path, options->plots_path, cost_fname, PATH_MAX); \
        } else { \
            snprintf(full_plot_path, PATH_MAX, "%s", plot_fname); \
            snprintf(full_costs_path, PATH_MAX, "%s", cost_fname); \
        }

    if (options->nn_params.enable) {
        NNConfig cfg = {
            .time_limit = options->nn_params.time_limit,
            .seed = options->inst.seed,
            .num_threads = (int) options->nn_params.num_threads
        };
        TspAlgorithm algo = nn_create(cfg);
        BUILD_PATHS(options->nn_params.plot_file, options->nn_params.cost_file);
        execute_and_report(&algo, instance, full_plot_path, full_costs_path, threads);
    }

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
        execute_and_report(&algo, instance, full_plot_path, full_costs_path, threads);
    }

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
        execute_and_report(&algo, instance, full_plot_path, full_costs_path, threads);
    }

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
        execute_and_report(&algo, instance, full_plot_path, full_costs_path, threads);
    }

    if (options->em_params.enable) {
        EMConfig cfg = {
            .time_limit = options->em_params.time_limit,
            .seed = options->inst.seed
        };
        TspAlgorithm algo = em_create(cfg);
        BUILD_PATHS(options->em_params.plot_file, options->em_params.cost_file);
        execute_and_report(&algo, instance, full_plot_path, full_costs_path, threads);
    }

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
        execute_and_report(&algo, instance, full_plot_path, full_costs_path, threads);
    }

    if (options->benders_params.enable) {
        BendersConfig cfg = {
            .time_limit = options->benders_params.time_limit,
            .max_iterations = (int) options->benders_params.max_iterations
        };
        TspAlgorithm algo = benders_create(cfg);
        BUILD_PATHS(options->benders_params.plot_file, options->benders_params.cost_file);
        execute_and_report(&algo, instance, full_plot_path, full_costs_path, threads);
    }

    if (options->bc_params.enable) {
        BranchCutConfig cfg = {
            .time_limit = options->bc_params.time_limit,
            .num_threads = (int) options->bc_params.num_threads
        };
        TspAlgorithm algo = branch_and_cut_create(cfg);
        BUILD_PATHS(options->bc_params.plot_file, options->bc_params.cost_file);
        execute_and_report(&algo, instance, full_plot_path, full_costs_path, threads);
    }

    if (options->hf_params.enable) {
        HardFixingConfig cfg = {
            .time_limit = options->hf_params.time_limit,
            .fixing_rate = options->hf_params.fixing_rate,
            .heuristic_time_ratio = options->hf_params.heuristic_ratio,
            .heuristic_type = parse_warm_start_heuristic(options->hf_params.heuristic_name),
            .seed = options->inst.seed,
            .heuristic_args = NULL
        };

        cfg.heuristic_args = create_heuristic_config(cfg.heuristic_type, options);

        TspAlgorithm algo = hard_fixing_create(cfg);
        BUILD_PATHS(options->hf_params.plot_file, options->hf_params.cost_file);
        execute_and_report(&algo, instance, full_plot_path, full_costs_path, threads);
    }

    if (options->lb_params.enable) {
        LocalBranchingConfig cfg = {
            .time_limit = options->lb_params.time_limit,
            .k = options->lb_params.k,
            .heuristic_time_ratio = options->lb_params.heuristic_ratio,
            .heuristic_type = parse_warm_start_heuristic(options->lb_params.heuristic_name),
            .seed = options->inst.seed,
            .heuristic_args = NULL
        };

        cfg.heuristic_args = create_heuristic_config(cfg.heuristic_type, options);

        TspAlgorithm algo = local_branching_create(cfg);
        BUILD_PATHS(options->lb_params.plot_file, options->lb_params.cost_file);
        execute_and_report(&algo, instance, full_plot_path, full_costs_path, threads);
    }

#undef BUILD_PATHS
}
