#include <math.h>
#include <stdlib.h>
#include <tsp_math_util.h>

#include "c_util.h"

double calculate_tour_cost(const int* const tour,
                           const int number_of_nodes,
                           const double* const edge_cost_array)
{
    double cost = 0;
    for (int i = 0; i < number_of_nodes; i++)
    {
        cost += edge_cost_array[tour[i] * number_of_nodes + tour[i + 1]];
    }
    return cost;
}

double normalized_rand() { return (double)rand() / RAND_MAX; }

double* init_edge_cost_array(const Node* nodes, const int number_of_nodes)
{
    double* const edge_cost_array = calloc(number_of_nodes * number_of_nodes, sizeof(double));
    const double edge_array_size = number_of_nodes * number_of_nodes;
    for (int i = 0; i < edge_array_size; i++)
    {
        const int row = i / number_of_nodes;
        const int colum = i % number_of_nodes;
        const double dx = nodes[row].x - nodes[colum].x;
        const double dy = nodes[row].y - nodes[colum].y;
        edge_cost_array[i] = sqrt(dx * dx + dy * dy);
    }

    return edge_cost_array;
}

/**
 * @brief Computes the cost change (delta) for a special n-opt move configuration
 *        where all segments are inverted except the first one.
 *
 * This function calculates the change in tour cost when performing an n-opt operation
 * with the specific pattern of inverting all segments except the first. The n-opt move
 * removes n edges from the tour and replaces them with n new edges.
 *
 * The algorithm works as follows:
 * 1. Calculates the cost of all edges that will be removed from the original tour
 * 2. Calculates the cost of all new edges that will be added to form the new tour
 * 3. Returns the difference (delta) between these costs
 *
 * @param number_of_segments Number of segments in the n-opt move (equals n)
 * @param tour Array representing the current tour configuration
 * @param edges_to_remove Array containing indices of the edges to be removed
 * @param edge_cost_array Matrix of edge costs between nodes (flattened 2D array)
 * @param number_of_nodes Total number of nodes in the problem
 *
 * @return The total cost change (delta) for the n-opt move. A negative value
 *         indicates an improvement in the tour cost.
 */
double compute_n_opt_cost(const int number_of_segments,
                          int tour[],
                          const int* edges_to_remove,
                          const double edge_cost_array[],
                          const int number_of_nodes)
{
    double delta = 0.0;

    // Subtract the cost of all edges that will be removed from the tour
    for (int i = 0; i < number_of_segments; i++)
    {
        const int start_node = edges_to_remove[i];
        const int end_node = start_node + 1;
        delta -= edge_cost_array[tour[start_node] + tour[end_node] * number_of_nodes];
    }

    int start_node = 0;
    int end_node = 0;

    // Add the cost of new connections between middle segments
    // These edges connect the end of one segment to the beginning of another after inversion
    for (int i = 1; i < number_of_segments - 1; i++)
    {
        start_node = edges_to_remove[i - 1] + 1;
        end_node = edges_to_remove[(i + 1) % number_of_segments];
        delta += edge_cost_array[tour[start_node] + tour[end_node] * number_of_nodes];
    }

    // Add the cost of connecting the first segment (not inverted) to the second segment
    start_node = edges_to_remove[0];
    end_node = edges_to_remove[1];
    delta += edge_cost_array[tour[start_node] + tour[end_node] * number_of_nodes];

    // Add the cost of connecting the last segment back to the first segment
    start_node = edges_to_remove[number_of_segments - 1];
    end_node = edges_to_remove[0];
    delta += edge_cost_array[tour[start_node] + tour[end_node] * number_of_nodes];

    // Return the total change in cost
    return delta;
}

/**
 * @brief Performs the actual n-opt move by inverting segments between removed edges
 *
 * This function implements the n-opt move by inverting (reversing) segments of the tour
 * between the edges that are to be removed. For each pair of consecutive edges to remove,
 * the function reverses the segment of the tour between them. This is part of the local
 * search algorithm for solving combinatorial optimization problems like the Traveling
 * Salesperson Problem (TSP).
 *
 * Note that the first edge in the original tour is removed as part of this operation,
 * which is important for understanding the resulting tour configuration.
 *
 * The function works in conjunction with compute_n_opt_cost which calculates the cost
 * change before the actual move is performed.
 *
 * @param number_of_edges_to_remove Number of edges to be removed in the n-opt move
 * @param tour Array representing the current tour configuration which will be modified
 * @param edges_to_remove Array containing indices of the edges to be removed
 * @param number_of_nodes
 */
void compute_n_opt_move(const int number_of_edges_to_remove,
                        int tour[],
                        const int* edges_to_remove,
                        const int number_of_nodes)
{
    // Process each segment except the last one
    for (int i = 0; i < number_of_edges_to_remove - 1; i += 1)
    {
        // Identify the start and end nodes of the segment to reverse
        const int start_node = edges_to_remove[i] + 1;
        const int end_node = edges_to_remove[i + 1];

        // Reverse the segment in the tour between start_node and end_node
        // This effectively inverts the segment's direction in the tour
        reverse_array_int(tour, tour[start_node], tour[end_node]);
    }
    tour[number_of_nodes] = tour[0];
}
