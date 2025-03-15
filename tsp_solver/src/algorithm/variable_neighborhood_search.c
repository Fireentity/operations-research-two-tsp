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

static void apply_vsn(const int starting_node,
                      int* tour,
                      const int number_of_nodes,
                      const double* edge_cost_array,
                      double* cost)
{

}

static void solve(const TspAlgorithm* tsp_algorithm,
                  int* tour,
                  const int number_of_nodes,
                  const double* edge_cost_array,
                  double* cost)
{
}

const TspAlgorithm* init_vns(const int kick_repetition, const double time_limit)
{
    const VariableNeighborhoodSearch vns = {
        .kick_repetition = kick_repetition,
        .time_limit = time_limit,
    };
    const TspExtendedAlgorithms extended_algorithms = {
        .variable_neighborhood_search = malloc_from_stack(&vns, sizeof(vns))
    };
    const TspAlgorithm tsp_algorithm = {
        .solve = solve,
        .extended_algorithms = malloc_from_stack(&extended_algorithms, sizeof(extended_algorithms)),
    };
    return malloc_from_stack(&tsp_algorithm, sizeof(tsp_algorithm));
}
