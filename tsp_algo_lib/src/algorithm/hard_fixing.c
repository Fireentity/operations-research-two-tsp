#include "hard_fixing.h"
#include "cplex_solver_wrapper.h"
#include "time_limiter.h"
#include "logger.h"
#include "c_util.h"
#include "random.h"
#include <stdlib.h>

#include "nearest_neighbor.h"
#include "extra_mileage.h"
#include "variable_neighborhood_search.h"
#include "tabu_search.h"
#include "grasp.h"

static void free_hf_config(void *cfg_void) {
    HardFixingConfig *cfg = cfg_void;
    if (cfg->heuristic_args) {
        free(cfg->heuristic_args);
    }
    free(cfg);
}

static void run_internal_heuristic(const TspInstance *inst,
                                   TspSolution *sol,
                                   CostRecorder *rec,
                                   const HardFixingConfig *hf_cfg,
                                   double sub_time_limit) {
    TspAlgorithm algo = {0};
    uint64_t seed = hf_cfg->seed;

    switch (hf_cfg->heuristic_type) {
        case HF_HEURISTIC_NN: {
            NNConfig cfg;
            if (hf_cfg->heuristic_args) {
                cfg = *(NNConfig *) hf_cfg->heuristic_args;
                cfg.time_limit = sub_time_limit;
            } else {
                cfg = (NNConfig){.time_limit = sub_time_limit, .seed = seed};
            }
            algo = nn_create(cfg);
            break;
        }
        case HF_HEURISTIC_EXTRA_MILEAGE: {
            EMConfig cfg;
            if (hf_cfg->heuristic_args) {
                cfg = *(EMConfig *) hf_cfg->heuristic_args;
                cfg.time_limit = sub_time_limit;
            } else {
                cfg = (EMConfig){.time_limit = sub_time_limit, .seed = seed};
            }
            algo = em_create(cfg);
            break;
        }
        case HF_HEURISTIC_TABU: {
            TabuConfig cfg;
            if (hf_cfg->heuristic_args) {
                cfg = *(TabuConfig *) hf_cfg->heuristic_args;
                cfg.time_limit = sub_time_limit;
            } else {
                cfg = (TabuConfig){
                    .min_tenure = 5, .max_tenure = 20, .max_stagnation = 200,
                    .time_limit = sub_time_limit, .seed = seed
                };
            }
            algo = tabu_create(cfg);
            break;
        }
        case HF_HEURISTIC_GRASP: {
            GraspConfig cfg;
            if (hf_cfg->heuristic_args) {
                cfg = *(GraspConfig *) hf_cfg->heuristic_args;
                cfg.time_limit = sub_time_limit;
            } else {
                cfg = (GraspConfig){
                    .rcl_size = 5, .probability = 0.2, .max_stagnation = 200,
                    .time_limit = sub_time_limit, .seed = seed
                };
            }
            algo = grasp_create(cfg);
            break;
        }
        case HF_HEURISTIC_VNS:
        default: {
            VNSConfig cfg;
            if (hf_cfg->heuristic_args) {
                cfg = *(VNSConfig *) hf_cfg->heuristic_args;
                cfg.time_limit = sub_time_limit;
            } else {
                cfg = (VNSConfig){
                    .min_k = 3, .max_k = 10, .kick_repetition = 1, .max_stagnation = 500,
                    .time_limit = sub_time_limit, .seed = seed
                };
            }
            algo = vns_create(cfg);
            break;
        }
    }

    if (algo.run) {
        tsp_algorithm_run(&algo, inst, sol, rec);
        tsp_algorithm_destroy(&algo);
    }
}

static void run_hard_fixing(const TspInstance *inst,
                            TspSolution *sol,
                            const void *cfg_void,
                            CostRecorder *rec) {
    const HardFixingConfig *cfg = cfg_void;
    int n = tsp_instance_get_num_nodes(inst);

    if_verbose(VERBOSE_INFO, "HardFixing: FixRate=%.2f, Time=%.2f\n",
               cfg->fixing_rate, cfg->time_limit);

    TimeLimiter timer = time_limiter_create(cfg->time_limit);
    time_limiter_start(&timer);

    double ratio = cfg->heuristic_time_ratio;
    if (ratio <= 0.0 || ratio >= 1.0) ratio = 0.2; // Default safe

    double heuristic_time = cfg->time_limit * ratio;

    if (heuristic_time < 2.0) heuristic_time = 2.0;
    if (heuristic_time > cfg->time_limit) heuristic_time = cfg->time_limit;

    if_verbose(VERBOSE_INFO, "HardFixing: Running warm-start (%.1fs)...\n", heuristic_time);

    run_internal_heuristic(inst, sol, rec, cfg, heuristic_time);

    if (time_limiter_is_over(&timer)) {
        if_verbose(VERBOSE_INFO, "HardFixing: Time over after heuristic.\n");
        return;
    }

#ifdef ENABLE_CPLEX
    CplexSolverContext *ctx = cplex_solver_create(inst);
    if (!ctx) return;

    if (cplex_solver_build_base_model(ctx, inst) != 0) {
        cplex_solver_destroy(ctx);
        return;
    }

    int *tour = malloc((n + 1) * sizeof(int));
    check_alloc(tour);
    tsp_solution_get_tour(sol, tour);

    int fixed_count = 0;
    RandomState rng;
    random_init(&rng, cfg->seed);

    for (int i = 0; i < n; i++) {
        int u = tour[i];
        int v = tour[i + 1];

        if (random_double(&rng) < cfg->fixing_rate) {
            cplex_solver_fix_edge(ctx, u, v, 1.0, n);
            fixed_count++;
        }
    }
    if_verbose(VERBOSE_INFO, "HardFixing: Fixed %d/%d edges (%.1f%%)\n",
               fixed_count, n, cfg->fixing_rate * 100.0);

    cplex_solver_add_mip_start(ctx, n, tour);
    cplex_solver_install_sec_callback(ctx, inst);

    double remaining = time_limiter_get_remaining(&timer);
    cplex_solver_set_time_limit(ctx, remaining);

    if (cplex_solver_optimize(ctx) == 0 && cplex_solver_has_solution(ctx)) {
        double cost = 0.0;
        cplex_solver_extract_solution(ctx, &cost);
        const double *x = cplex_solver_get_x(ctx);

        cplex_solver_reconstruct_tour(n, x, tour);
        tsp_solution_update_if_better(sol, tour, cost);
        cost_recorder_add(rec, cost);

        if_verbose(VERBOSE_INFO, "HardFixing: CPLEX improved solution to %.2f\n", cost);
    } else {
        if_verbose(VERBOSE_INFO, "HardFixing: CPLEX found no improvement or timed out.\n");
    }

    free(tour);
    cplex_solver_destroy(ctx);
#else
    if_verbose(VERBOSE_INFO, "HardFixing: CPLEX not available, returning heuristic solution.\n");
#endif
}

TspAlgorithm hard_fixing_create(HardFixingConfig config) {
    HardFixingConfig *c = malloc(sizeof(HardFixingConfig));
    check_alloc(c);
    *c = config;

    return (TspAlgorithm){
        .name = "Hard Fixing Matheuristic",
        .run = run_hard_fixing,
        .config = c,
        .free_config = free_hf_config
    };
}
