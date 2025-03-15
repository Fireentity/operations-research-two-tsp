#include <chrono.h>
#include <variable_neighborhood_search.h>
#include <c_util.h>
#include <float.h>
#include <nearest_neighbor.h>
#include <stdlib.h>
#include <string.h>
#include <tsp_math_util.h>

union TspExtendedAlgorithms
{
    VariableNeighborhoodSearch* variable_neighborhood_search;
};

static void n_opt(int* tour,
                  const int number_of_nodes,
                  const double* edge_cost_array,
                  double* cost,
                  const double time_limit,
                  const int n_opt)
{
    const double start_time = second();
    int improved = 1;
    while (improved && (second() - start_time < time_limit)) {
        improved = 0;
        for (int i = 1; i < number_of_nodes - 1; i++) {
            int found = 0;
            for (int k = i + 1; k < number_of_nodes; k++) {
                const int a = tour[i - 1];
                const int b = tour[i];
                const int c = tour[k];
                const int d = tour[(k + 1) % number_of_nodes];
                const double delta = edge_cost_array[a * number_of_nodes + c] +
                               edge_cost_array[b * number_of_nodes + d] -
                               edge_cost_array[a * number_of_nodes + b] -
                               edge_cost_array[c * number_of_nodes + d];
                if (delta < 0) {
                    REVERSE_ARRAY(tour, i, k);
                    *cost += delta;
                    improved = 1;
                    found = 1;
                    break;
                }
            }
            if (found)
                break;
        }
        if (!improved && n_opt > 2) {
            int i = 1 + rand() % (number_of_nodes - 3);
            int j = i + 1 + rand() % (number_of_nodes - i - 2);
            int k = j + 1 + rand() % (number_of_nodes - j - 1);
            int new_tour[number_of_nodes + 1];
            memcpy(new_tour, tour, sizeof(int) * (number_of_nodes + 1));
            REVERSE_ARRAY(new_tour, i, j);
            REVERSE_ARRAY(new_tour, j + 1, k);
            const double new_cost = calculate_tour_cost(new_tour, number_of_nodes, edge_cost_array);
            if (new_cost < *cost) {
                memcpy(tour, new_tour, sizeof(int) * (number_of_nodes + 1));
                *cost = new_cost;
                improved = 1;
            }
        }
    }
}

static void apply_vsn(const int starting_node,
                      int* tour,
                      const int number_of_nodes,
                      const double* edge_cost_array,
                      double* cost)
{
    if (starting_node >= number_of_nodes) {
        printf("The starting node (%d) cannot be greater than or equal to the number of nodes (%d)\n",
               starting_node, number_of_nodes);
        exit(EXIT_FAILURE);
    }
    SWAP(tour[0], tour[starting_node]);
    for (int i = 0; i < starting_node; i++) {
        int a = rand() % number_of_nodes;
        int b = rand() % number_of_nodes;
        SWAP(tour[a], tour[b]);
    }
    tour[number_of_nodes] = tour[0];
    *cost = calculate_tour_cost(tour, number_of_nodes, edge_cost_array);
}

static void solve(const TspAlgorithm* tsp_algorithm,
                  int* tour,
                  const int number_of_nodes,
                  const double* edge_cost_array,
                  double* cost)
{
    const VariableNeighborhoodSearch* vns = tsp_algorithm->extended_algorithms->variable_neighborhood_search;
    double best_solution_cost = DBL_MAX;
    const double start_time = second();
    int best_tour[number_of_nodes + 1];
    memcpy(best_tour, tour, sizeof(int) * (number_of_nodes + 1));
    int iteration = 0;
    int current_neighborhood = 2;
    while ((second() - start_time) < vns->time_limit) {
        apply_vsn(iteration % number_of_nodes, tour, number_of_nodes, edge_cost_array, cost);
        n_opt(tour, number_of_nodes, edge_cost_array, cost, vns->time_limit, current_neighborhood);
        if (*cost < best_solution_cost) {
            best_solution_cost = *cost;
            memcpy(best_tour, tour, sizeof(int) * (number_of_nodes + 1));
            current_neighborhood = 2;
        } else {
            current_neighborhood++;
            if (current_neighborhood > vns->kick_repetition) {
                current_neighborhood = 2;
                for (int i = 0; i < vns->kick_repetition; i++) {
                    const int a = rand() % number_of_nodes;
                    const int b = rand() % number_of_nodes;
                    SWAP(tour[a], tour[b]);
                }
                *cost = calculate_tour_cost(tour, number_of_nodes, edge_cost_array);
            }
        }
        iteration++;
    }
    memcpy(tour, best_tour, sizeof(int) * (number_of_nodes + 1));
}

const TspAlgorithm* init_vns(const int kick_repetition, const double time_limit)
{
    VariableNeighborhoodSearch vns = {
        .kick_repetition = kick_repetition,
        .time_limit = time_limit,
    };
    TspExtendedAlgorithms extended_algorithms = {
        .variable_neighborhood_search = MALLOC_FROM_STACK(vns)
    };
    const TspAlgorithm tsp_algorithm = {
        .solve = solve,
        .extended_algorithms = MALLOC_FROM_STACK(extended_algorithms),
    };
    return MALLOC_FROM_STACK(tsp_algorithm);
}
