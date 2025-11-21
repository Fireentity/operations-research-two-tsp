#include <float.h>
#include <stdio.h>
#include "c_util.h"
#include "algorithms.h"
#include "constants.h"
#include "time_limiter.h"
#include "tsp_math_util.h"
#include "logger.h"
#include <math.h>
/**
 * @brief Performs a 2-opt optimization on a given tour (First Improvement Type).
 */
inline double two_opt(int* tour,
                      const int number_of_nodes,
                      const double* edge_cost_array,
                      const TimeLimiter* time_limiter,
                      const double epsilon) {
    double cost_improvement = 0;
    bool improved = true;

    while (improved) {
        improved = false;

        for (int i = 1; i < number_of_nodes - 1; i++) {
            if (time_limiter_is_over(time_limiter)) {
                if_verbose(VERBOSE_DEBUG, "  2-Opt: Time limit reached during optimization. Total improvement: %lf\n",
                           cost_improvement);
                return cost_improvement;
            }
            for (int j = i + 1; j < number_of_nodes; j++) {
                if (i == 1 && j == number_of_nodes - 1)
                    continue;

                const int a = tour[i - 1];
                const int b = tour[i];
                const int c = tour[j];
                const int d = tour[j + 1];

                const double delta = edge_cost_array[a * number_of_nodes + c] +
                    edge_cost_array[b * number_of_nodes + d] -
                    (edge_cost_array[a * number_of_nodes + b] +
                        edge_cost_array[c * number_of_nodes + d]);

                if (delta < -epsilon) {
                    cost_improvement += delta;
                    reverse_array_int(tour, i, j);
                    improved = true;
                    break; // Break inner loop
                }
            }
            if (improved) break; // Break outer loop to restart from i = 1
        }
        // If no improvement was found in the full pass, the outer loop will terminate
    }

    if_verbose(VERBOSE_ALL, "  2-Opt: Finished local search. Total improvement: %lf\n", cost_improvement);
    return cost_improvement;
}


/**
 * @brief Generates a nearest neighbor tour starting from a given node.
 */
int nearest_neighbor_tour(const int starting_node,
                          int* tour,
                          const int number_of_nodes,
                          const double* edge_cost_array,
                          double* cost) {
    // Corrected bounds check
    if (starting_node < 0 || starting_node >= number_of_nodes) {
        if_verbose(VERBOSE_INFO, "[ERROR] NN: Starting node (%d) is out of bounds [0, %d).\n",
                   starting_node, number_of_nodes);
        fprintf(stderr, "[ERROR] NN: Starting node (%d) is out of bounds [0, %d).\n",
                starting_node, number_of_nodes);
        return -1;
    }

    if_verbose(VERBOSE_DEBUG, "    NN: Building tour from start node %d...\n", starting_node);

    // FIX: Initialize the tour array, this was missing
    for (int i = 0; i < number_of_nodes; i++) {
        tour[i] = i;
    }

    swap_int(tour, 0, starting_node);
    int visited_count = 1;
    int current_node = tour[0];
    tour[number_of_nodes] = tour[0];

    while (visited_count < number_of_nodes) {
        double best_cost = DBL_MAX;
        int best_index = visited_count;

        for (int i = visited_count; i < number_of_nodes; i++) {
            const double cost_candidate = edge_cost_array[current_node * number_of_nodes + tour[i]];
            if (cost_candidate < best_cost) {
                best_cost = cost_candidate;
                best_index = i;
            }
        }

        swap_int(tour, visited_count, best_index);
        current_node = tour[visited_count];
        visited_count++;
    }

    *cost = calculate_tour_cost(tour, number_of_nodes, edge_cost_array);
    if_verbose(VERBOSE_DEBUG, "    NN: Built tour. Cost: %lf\n", *cost);
    return 0;
}


/**
 * @brief Generates a tour using a modified nearest neighbor approach with probabilistic selection.
 */
int grasp_nearest_neighbor_tour(const int starting_node,
                                int* tour,
                                const int number_of_nodes,
                                const double* edge_cost_array,
                                double* cost,
                                const double p1,
                                const double p2,
                                const double p3) {
    // Corrected bounds check
    if (starting_node < 0 || starting_node >= number_of_nodes) {
        if_verbose(VERBOSE_INFO, "[ERROR] GRASP-NN: Starting node (%d) is out of bounds [0, %d).\n",
                   starting_node, number_of_nodes);
        fprintf(stderr, "[ERROR] GRASP-NN: Starting node (%d) is out of bounds [0, %d).\n",
                starting_node, number_of_nodes);
        return -1;
    }

    if_verbose(VERBOSE_DEBUG, "    GRASP-NN: Building tour from start node %d (p1=%.2f, p2=%.2f)...\n",
               starting_node, p1, p2);

    // This initialization is correct and necessary
    for (int i = 0; i < number_of_nodes; i++) {
        tour[i] = i;
    }
    swap_int(tour, 0, starting_node);

    int visited_count = 1;
    int current_node = tour[0];
    tour[number_of_nodes] = tour[0];

    while (visited_count < number_of_nodes) {
        int min_index[4] = {-1, -1, -1, -1};
        double min_dist[4] = {DBL_MAX, DBL_MAX, DBL_MAX, DBL_MAX};

        for (int i = visited_count; i < number_of_nodes; i++) {
            const int candidate = tour[i];
            const double dist = edge_cost_array[current_node * number_of_nodes + candidate];

            for (int k = 0; k < 4; k++) {
                if (dist < min_dist[k]) {
                    for (int shift = 3; shift > k; shift--) {
                        min_dist[shift] = min_dist[shift - 1];
                        min_index[shift] = min_index[shift - 1];
                    }
                    min_dist[k] = dist;
                    min_index[k] = i;
                    break;
                }
            }
        }

        int candidates_found = 0;
        for (int k = 0; k < 4; k++) {
            if (min_index[k] >= 0) {
                candidates_found++;
            } else {
                break;
            }
        }

        // Safety check
        if (candidates_found == 0) {
            if_verbose(VERBOSE_INFO, "[ERROR] GRASP-NN: No candidates found, loop stuck.\n");
            return -1; // Prevent infinite loop or crash
        }

        int chosen_index;
        const double r = normalized_rand();

        if (r < p1) {
            chosen_index = min_index[0];
        } else {
            if (candidates_found >= 4) {
                if (r < p2) {
                    chosen_index = min_index[1];
                } else if (r < p2 + p3) {
                    chosen_index = min_index[2];
                } else {
                    chosen_index = min_index[3];
                }
            } else {
                if (candidates_found == 3) {
                    const double p_sum = p2 + p3;
                    // Add divide-by-zero protection
                    if (p_sum < EPSILON) chosen_index = min_index[1]; // Default to 2nd nearest
                    else if (r < p2 / p_sum) chosen_index = min_index[1];
                    else chosen_index = min_index[2];
                } else if (candidates_found == 2) {
                    chosen_index = min_index[1];
                } else {
                    // candidates_found == 1
                    chosen_index = min_index[0];
                }
            }
        }

        if_verbose(VERBOSE_DEBUG, "      GRASP-NN: Chose node %d (RCL size %d, r=%.2f).\n", tour[chosen_index],
                   candidates_found, r);
        swap_int(tour, visited_count, chosen_index);
        current_node = tour[visited_count];
        visited_count++;
    }

    *cost = calculate_tour_cost(tour, number_of_nodes, edge_cost_array);
    if_verbose(VERBOSE_DEBUG, "    GRASP-NN: Built tour. Cost: %lf\n", *cost);
    return 0;
}

int grasp_nearest_neighbor_tour_threshold(const int starting_node,
                                          int* tour,
                                          const int number_of_nodes,
                                          const double* edge_cost_array,
                                          double* cost,
                                          const double alpha) {
    if (starting_node < 0 || starting_node >= number_of_nodes) {
        if_verbose(VERBOSE_INFO,
                   "[ERROR] GRASP-Threshold: Starting node (%d) is out of bounds [0, %d).\n",
                   starting_node, number_of_nodes);
        fprintf(stderr,
                "[ERROR] GRASP-Threshold: Starting node (%d) is out of bounds [0, %d).\n",
                starting_node, number_of_nodes);
        return -1;
    }

    if_verbose(VERBOSE_DEBUG,
               "    GRASP-Threshold: Building tour from node %d (alpha=%.2f)...\n",
               starting_node, alpha);

    // initialize tour permutation
    for (int i = 0; i < number_of_nodes; i++)
        tour[i] = i;
    swap_int(tour, 0, starting_node);

    int visited_count = 1;
    int current_node = tour[0];
    tour[number_of_nodes] = tour[0];

    // RCL allocation
    int* rcl = malloc(number_of_nodes * sizeof(int));
    check_alloc(rcl);

    // clamp alpha
    const double effective_alpha = fmax(0.0, fmin(1.0, alpha));

    while (visited_count < number_of_nodes) {

        double min_cost = DBL_MAX;
        double max_cost = -DBL_MAX;

        // compute min/max cost among remaining nodes
        for (int i = visited_count; i < number_of_nodes; i++) {
            const int candidate = tour[i];
            const double dist = edge_cost_array[current_node * number_of_nodes + candidate];
            if (dist < min_cost) min_cost = dist;
            if (dist > max_cost) max_cost = dist;
        }

        if (min_cost == DBL_MAX) {
            if_verbose(VERBOSE_INFO,
                       "[ERROR] GRASP-Threshold: No candidates found.\n");
            free(rcl);
            return -1;
        }

        // threshold for RCL
        const double threshold = min_cost + effective_alpha * (max_cost - min_cost);

        // build RCL
        int rcl_size = 0;
        for (int i = visited_count; i < number_of_nodes; i++) {
            const int candidate = tour[i];
            const double dist =
                edge_cost_array[current_node * number_of_nodes + candidate];
            if (dist <= threshold + EPSILON)
                rcl[rcl_size++] = i;
        }

        // random pick
        const int chosen_index = rcl[(int)(normalized_rand() * rcl_size)];

        if_verbose(VERBOSE_DEBUG,
                   "      GRASP-Threshold: Chose node %d (RCL size %d).\n",
                   tour[chosen_index], rcl_size);

        // commit choice
        swap_int(tour, visited_count, chosen_index);
        current_node = tour[visited_count];
        visited_count++;
    }

    free(rcl);
    *cost = calculate_tour_cost(tour, number_of_nodes, edge_cost_array);

    if_verbose(VERBOSE_DEBUG,
               "    GRASP-Threshold: Built tour. Cost: %lf\n",
               *cost);

    return 0;
}
