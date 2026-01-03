#include "matheuristic_utils.h"
#include "nearest_neighbor.h"
#include "extra_mileage.h"
#include "variable_neighborhood_search.h"
#include "tabu_search.h"
#include "grasp.h"
#include "genetic.h"
#include <stdlib.h>

#include "c_util.h"

void matheuristic_free_args(void *heuristic_args) {
    tsp_free(heuristic_args);
}

void matheuristic_run_warm_start(const WarmStartParams *params,
                                 const TspInstance *inst,
                                 TspSolution *sol,
                                 CostRecorder *rec) {
    TspAlgorithm algo = {0};
    uint64_t seed = params->seed;
    double time = params->time_limit;

    switch (params->heuristic_type) {
        case NN: {
            NNConfig c = params->heuristic_args ? *(NNConfig *) params->heuristic_args : (NNConfig){0};
            c.time_limit = time;
            c.seed = seed;
            algo = nn_create(c);
            //TODO needs num of threads?
            break;
        }
        case EM: {
            EMConfig c = params->heuristic_args ? *(EMConfig *) params->heuristic_args : (EMConfig){0};
            c.time_limit = time;
            c.seed = seed;
            algo = em_create(c);
            break;
        }
        case TABU: {
            TabuConfig c;
            if (params->heuristic_args) {
                c = *(TabuConfig *) params->heuristic_args;
                c.time_limit = time;
            } else {
                c = (TabuConfig){
                    .min_tenure = 5, .max_tenure = 20, .max_stagnation = 200, .time_limit = time, .seed = seed
                };
            }
            algo = tabu_create(c);
            break;
        }
        case GRASP: {
            GraspConfig c;
            if (params->heuristic_args) {
                c = *(GraspConfig *) params->heuristic_args;
                c.time_limit = time;
            } else {
                c = (GraspConfig){
                    .rcl_size = 5, .probability = 0.2, .max_stagnation = 200, .time_limit = time, .seed = seed
                };
            }
            algo = grasp_create(c);
            break;
        }
        case GENETIC: {
            GeneticConfig c;
            if (params->heuristic_args) {
                c = *(GeneticConfig *) params->heuristic_args;
                c.time_limit = time;
            } else {
                c = (GeneticConfig){
                    .time_limit = time, .seed = seed, .population_size = 100, .elite_count = 1, .mutation_rate = 0.1,
                    .crossover_cut_min_ratio = 25, .crossover_cut_max_ratio = 75, .tournament_size = 5,
                    .init_grasp_percent = 100
                };
            }
            algo = genetic_create(c);
            break;
        }
        case VNS:
        default: {
            VNSConfig c;
            if (params->heuristic_args) {
                c = *(VNSConfig *) params->heuristic_args;
                c.time_limit = time;
            } else {
                c = (VNSConfig){
                    .min_k = 3, .max_k = 10, .kick_repetition = 1, .max_stagnation = 500, .time_limit = time,
                    .seed = seed
                };
            }
            algo = vns_create(c);
            break;
        }
    }

    if (algo.run) {
        tsp_algorithm_run(&algo, inst, sol, rec);
        tsp_algorithm_destroy(&algo);
    }
}
