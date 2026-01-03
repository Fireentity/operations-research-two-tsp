#include "nearest_neighbor.h"
#include "time_limiter.h"
#include "c_util.h"
#include "logger.h"
#include <string.h>
#include <pthread.h>

#include "constructive.h"
#include "tsp_math.h"
#include "local_search.h"

typedef struct {
    int thread_id;
    int num_threads;
    int num_nodes;
    const double *costs;
    double time_limit;

    TspSolution *solution;
    CostRecorder *local_recorder;
} NNWorkerArgs;

static void *nn_worker(const void *arg) {
    const NNWorkerArgs *args = arg;

    TimeLimiter timer = time_limiter_create(args->time_limit);
    time_limiter_start(&timer);

    int *tour = tsp_malloc((args->num_nodes + 1) * sizeof(int));

    // Deterministic partitioning of the starting nodes
    const int chunk_size = args->num_nodes / args->num_threads;
    const int start_node = args->thread_id * chunk_size;
    const int end_node = args->thread_id == args->num_threads - 1
                             ? args->num_nodes
                             : (args->thread_id + 1) * chunk_size;

    for (int s = start_node; s < end_node; s++) {
        if (time_limiter_is_over(&timer)) break;

        double cost;

        if (nearest_neighbor_tour(s, tour, args->num_nodes, args->costs, &cost) == 0) {
            cost += two_opt(tour, args->num_nodes, args->costs, timer);

            // Record locally (no mutex needed)
            cost_recorder_add(args->local_recorder, cost);

            // Update global solution (TspSolution is thread-safe)
            if (tsp_solution_update_if_better(args->solution, tour, cost)) {
                if_verbose(VERBOSE_DEBUG,
                           "\tNN [Thread %d]: new best %.2f from start %d\n",
                           args->thread_id, cost, s);
            }
        }
    }

    tsp_free(tour);
    return NULL;
}

static void run_nn(const TspInstance *instance,
                   TspSolution *solution,
                   const void *config_void,
                   CostRecorder *recorder) {
    const NNConfig *cfg = config_void;
    const int n = tsp_instance_get_num_nodes(instance);
    const double *costs = tsp_instance_get_cost_matrix(instance);

    // 0 implies auto-detect, otherwise use user value
    int num_threads = cfg->num_threads > 0 ? cfg->num_threads : (int) get_max_threads();
    if (num_threads > n) num_threads = n;

    if_verbose(VERBOSE_INFO,
               "NN: Running parallel search with %d threads (Time limit: %.2f)\n",
               num_threads, cfg->time_limit);

    pthread_t *threads = tsp_malloc(num_threads * sizeof(pthread_t));
    NNWorkerArgs *args = tsp_malloc(num_threads * sizeof(NNWorkerArgs));

    for (int i = 0; i < num_threads; i++) {
        // Create a local recorder for each thread to avoid contention
        CostRecorder *local_rec = cost_recorder_create(n / num_threads + 16);

        args[i] = (NNWorkerArgs){
            .thread_id = i,
            .num_threads = num_threads,
            .num_nodes = n,
            .costs = costs,
            .time_limit = cfg->time_limit,
            .solution = solution,
            .local_recorder = local_rec
        };

        if (pthread_create(&threads[i], NULL, (void *) nn_worker, &args[i]) != 0) {
            if_verbose(VERBOSE_INFO, "[ERROR] NN: Failed to create thread %d\n", i);
            cost_recorder_destroy(local_rec);
            args[i].local_recorder = NULL;
        }
    }

    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);

        if (recorder && args[i].local_recorder) {
            cost_recorder_merge(recorder, args[i].local_recorder);
        }

        if (args[i].local_recorder) {
            cost_recorder_destroy(args[i].local_recorder);
        }
    }

    tsp_free(args);
    tsp_free(threads);
}

static void free_nn_config(void *config) {
    tsp_free(config);
}

TspAlgorithm nn_create(const NNConfig config) {
    NNConfig *cpy = tsp_malloc(sizeof(NNConfig));
    *cpy = config;

    return (TspAlgorithm){
        .name = "Nearest Neighbor",
        .config = cpy,
        .run = run_nn,
        .free_config = free_nn_config,
        .clone_config = NULL // Internal parallelization
    };
}
