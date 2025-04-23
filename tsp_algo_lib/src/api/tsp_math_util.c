#include <math.h>
#include <stdlib.h>
#include "c_util.h"
#include "tsp_instance.h"

double calculate_tour_cost(const int* const tour,
                           const int number_of_nodes,
                           const double* const edge_cost_array)
{
    double cost = 0;
    // 'tour' is expected to have (number_of_nodes + 1) elements with the first and last node equal.
    for (int i = 0; i < number_of_nodes; i++)
    {
        // Access edge cost from node tour[i] to tour[i+1]
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
        // Calculate Euclidean distance between nodes at positions row and colum.
        edge_cost_array[i] = sqrt(dx * dx + dy * dy);
    }
    return edge_cost_array;
}


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
    start_node = edges_to_remove[number_of_segments - 2] + 1;
    end_node = edges_to_remove[number_of_segments - 1] + 1;
    delta += edge_cost_array[tour[start_node] + tour[end_node] * number_of_nodes];

    // Return the total change in cost
    return delta;
}


void compute_n_opt_move(const int number_of_edges_to_remove,
                        int tour[],
                        const int* edges_to_remove,
                        const int number_of_nodes)
{
    // Process each segment except the last one
    for (int i = 0; i < number_of_edges_to_remove - 1; i++)
    {
        // Identify the start and end nodes of the segment to reverse
        const int start_node = edges_to_remove[i] + 1;
        const int end_node = edges_to_remove[i + 1];

        // Reverse the segment in the tour between start_node and end_node
        // This effectively inverts the segment's direction in the tour
        reverse_array_int(tour, start_node, end_node);
    }
    tour[number_of_nodes] = tour[0];
}
