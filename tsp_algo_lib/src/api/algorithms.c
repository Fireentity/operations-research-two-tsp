#include <float.h>
#include "c_util.h"
#include "algorithms.h"
#include "time_limiter.h"
#include "tsp_math_util.h"


/**
 * @brief Performs a 2-opt optimization on a given tour.
 *
 * This function attempts to improve the tour by reversing segments of the tour if it results
 * in a decrease in the total cost. It iterates over the tour, looking for possible 2-opt moves
 * that improve the tour cost. If a move is found, the segment is reversed, and the process is repeated.
 * The function stops if the time limit is exceeded or no further improvements can be found.
 *
 * @param tour A pointer to the array representing the tour. The array should have a size of `number_of_nodes + 1`.
 * @param number_of_nodes The number of nodes in the tour (excluding the return to the starting point).
 * @param edge_cost_array A pointer to the array representing the cost of the edges between nodes.
 * @param time_limiter A pointer to a `TimeLimiter` object that checks if the time limit has been exceeded.
 * @param epsilon The threshold for considering improvements. If the improvement is smaller than this value, it will be ignored.
 *
 * @return The total cost improvement achieved through the 2-opt optimization.
 */
inline double two_opt(int *tour,
                      const int number_of_nodes,
                      const double *edge_cost_array,
                      const TimeLimiter *time_limiter,
                      const double epsilon) {
    double cost_improvement = 0;

    // It is assumed that tour has a size of number_of_nodes + 1.
    int i = 1;
    while (i < number_of_nodes - 1) {
        if (time_limiter->is_time_over(time_limiter))
            return cost_improvement;

        bool improvement_found = false;
        // j represents the index of the end of the segment to be reversed
        for (int j = i + 1; j < number_of_nodes; j++) {
            // Avoids the move that would reverse the entire tour
            if (i == 1 && j == number_of_nodes - 1)
                continue;

            const int a = tour[i - 1];
            const int b = tour[i];
            const int c = tour[j];
            const int d = tour[j + 1]; // tour has a size of number_of_nodes + 1

            // Optimized delta calculation:
            // delta = (cost of edge a-c + cost of edge b-d) - (cost of edge a-b + cost of edge c-d)
            const double delta = edge_cost_array[a * number_of_nodes + c] +
                                 edge_cost_array[b * number_of_nodes + d] -
                                 (edge_cost_array[a * number_of_nodes + b] +
                                  edge_cost_array[c * number_of_nodes + d]);

            // If the move leads to an improvement
            if (delta < -epsilon) {
                cost_improvement += delta;
                // Reverses the segment between indices i and j using the defined macro
                reverse_array_int(tour, i, j);
                improvement_found = true;
                break;
            }
        }

        // If an improvement was made, restart from i = 1
        i = improvement_found ? 1 : i + 1;
    }

    return cost_improvement;
}


/**
 * @brief Generates a nearest neighbor tour starting from a given node.
 *
 * This function generates a tour of nodes by selecting the nearest unvisited node at each step
 * starting from a given starting node. The function computes the total cost of the generated tour.
 * The tour is represented by an array of node indices, and the cost of the tour is computed based on
 * the edge costs between nodes.
 *
 * @param starting_node The node from which the tour starts. This node must be a valid node index.
 * @param tour An array to store the resulting tour. The array must have at least `number_of_nodes + 1` elements
 *             to store the full tour including the closing node.
 * @param number_of_nodes The total number of nodes to visit in the tour.
 * @param edge_cost_array A 2D array (flattened to 1D) that stores the costs of traveling between any two nodes.
 *                        The cost between nodes i and j is stored at `edge_cost_array[i * number_of_nodes + j]`.
 * @param cost A pointer to a variable where the total cost of the generated tour will be stored.
 *
 * @note The function modifies the `tour` array in place to represent the order of nodes visited.
 *       The tour is closed by adding the starting node at the end of the tour array.
 *
 * @return void
 */
void nearest_neighbor_tour(const int starting_node,
                           int *tour,
                           const int number_of_nodes,
                           const double *edge_cost_array,
                           double *cost) {
    if (starting_node > number_of_nodes) {
        printf("The starting node (%d) cannot be greater than the number of nodes (%d)",
               starting_node, number_of_nodes);
        exit(EXIT_FAILURE);
    }

    swap_int(tour, 0, starting_node);

    int visited_count = 1;
    int current_node = tour[0];

    // Close the tour by setting the last node to be the same as the first node
    tour[number_of_nodes] = tour[0];

    while (visited_count < number_of_nodes) {
        double best_cost = DBL_MAX;
        int best_index = visited_count;

        // Find the nearest unvisited node
        for (int i = visited_count; i < number_of_nodes; i++) {
            const double cost_candidate = edge_cost_array[current_node * number_of_nodes + tour[i]];
            if (cost_candidate < best_cost) {
                best_cost = cost_candidate;
                best_index = i;
            }
        }

        // Move the best found node to the next position in the tour
        swap_int(tour, visited_count, best_index);
        current_node = tour[visited_count];
        visited_count++;
    }

    // Compute the total cost of the generated tour
    *cost = calculate_tour_cost(tour, number_of_nodes, edge_cost_array);
}


/**
 * @brief Generates a tour using a modified nearest neighbor approach with probabilistic selection.
 *
 * The function modifies the classic nearest neighbor algorithm. Instead of always selecting the nearest unvisited node,
 * it probabilistically chooses from the 2nd, 3rd, or 4th nearest nodes, with probabilities p2, p3, and p4, respectively (if possible).
 *
 * @param starting_node The index of the starting node for the tour.
 * @param tour Pointer to an array where the tour will be stored. The array should be of size `number_of_nodes + 1`.
 * @param number_of_nodes The total number of nodes in the graph.
 * @param edge_cost_array A 2D array representing the cost (or distance) between each pair of nodes. The size should be
 *                        `number_of_nodes * number_of_nodes`.
 * @param cost Pointer to a double where the total cost of the generated tour will be stored.
 * @param p1 Probability of selecting the nearest node.
 * @param p2 Probability of selecting the 2nd nearest node.
 * @param p3 Probability of selecting the 3rd nearest node.
 *
 * @note The starting node must be less than `number_of_nodes`. The array `tour` should be pre-allocated with at least
 *       `number_of_nodes + 1` elements. The last element of the tour will be set to the starting node to close the cycle.
 */
void grasp_nearest_neighbor_tour(const int starting_node,
                                 int *tour,
                                 const int number_of_nodes,
                                 const double *edge_cost_array,
                                 double *cost,
                                 const double p1,
                                 const double p2,
                                 const double p3) {
    if (starting_node >= number_of_nodes) {
        printf("The starting node (%d) cannot be >= the number of nodes (%d)\n",
               starting_node, number_of_nodes);
        exit(EXIT_FAILURE);
    }

    swap_int(tour, 0, starting_node);

    // Track the number of visited nodes in the tour
    int visited_count = 1;
    int current_node = tour[0];

    // Close the tour (make it a cycle)
    tour[number_of_nodes] = tour[0];

    // Continue until all nodes have been visited
    while (visited_count < number_of_nodes) {
        // Find the 4 closest unvisited nodes

        // Track the indices and distances of the 4 closest nodes
        int min_index[4] = {-1, -1, -1, -1};
        double min_dist[4] = {DBL_MAX, DBL_MAX, DBL_MAX, DBL_MAX};

        // Loop through unvisited nodes to find the 4 smallest distances
        for (int i = visited_count; i < number_of_nodes; i++) {
            const int candidate = tour[i];
            const double dist = edge_cost_array[current_node * number_of_nodes + candidate];

            // Insert (candidate, dist) into the min_dist[] / min_index[] "top-4" list
            for (int k = 0; k < 4; k++) {
                if (dist < min_dist[k]) {
                    // Shift the others down
                    for (int shift = 3; shift > k; shift--) {
                        min_dist[shift] = min_dist[shift - 1];
                        min_index[shift] = min_index[shift - 1];
                    }
                    // Insert the new entry
                    min_dist[k] = dist;
                    min_index[k] = i;
                    break;
                }
            }
        }

        // Count how many valid nearest candidates we actually have
        int candidates_found = 0;
        for (int k = 0; k < 4; k++) {
            if (min_index[k] >= 0) {
                candidates_found++;
            } else {
                break;
            }
        }

        // Pick the first node with probability p1
        int chosen_index;
        const double r = normalized_rand();

        if (r < p1 && candidates_found > 0) {
            chosen_index = min_index[0]; // First node is chosen with probability p1
        } else {
            // If the first node is not chosen, pick among the remaining candidates
            if (candidates_found >= 4) {
                // We have at least 4 unvisited nodes
                if (r < p2) {
                    chosen_index = min_index[1]; // 2nd nearest
                } else if (r < p2 + p3) {
                    chosen_index = min_index[2]; // 3rd nearest
                } else {
                    chosen_index = min_index[3]; // 4th nearest
                }
            } else {
                // Fewer than 4 nodes remain unvisited. Fallback logic:
                if (candidates_found == 3) {
                    const double p_sum = p2 + p3;
                    if (r < p2 / p_sum) {
                        chosen_index = min_index[1];
                    } else {
                        chosen_index = min_index[2];
                    }
                } else if (candidates_found == 2) {
                    chosen_index = min_index[1];
                } else {
                    chosen_index = min_index[0]; // Only 1 candidate left
                }
            }
        }

        // "Visit" the chosen node by swapping it into the 'visited_count' position of the tour array.
        swap_int(tour, visited_count, chosen_index);

        // Advance to the next node
        current_node = tour[visited_count];
        visited_count++;
    }

    // Compute the total cost of the generated tour
    *cost = calculate_tour_cost(tour, number_of_nodes, edge_cost_array);
}
