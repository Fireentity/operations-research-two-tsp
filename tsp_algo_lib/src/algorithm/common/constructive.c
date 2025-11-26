#include "constructive.h"
#include "constants.h"
#include "c_util.h"
#include "logger.h"
#include "tsp_math.h"
#include <tgmath.h>
#include <float.h>

/**
 * @brief Generates a nearest neighbor tour starting from a given node.
 */
int nearest_neighbor_tour(const int starting_node,
                          int *tour,
                          const int n,
                          const double *costs,
                          double *cost) {
    /* Invalid starting node is reported for debugging clarity */
    if (starting_node < 0 || starting_node >= n) {
        if_verbose(VERBOSE_INFO,
                   "[ERROR] NN: starting node %d out of bounds [0,%d)\n",
                   starting_node, n);
        return -1;
    }

    if_verbose(VERBOSE_DEBUG, "\tNN: constructing tour from start %d\n", starting_node);

    for (int i = 0; i < n; i++)
        tour[i] = i;

    swap_int(tour, tour + starting_node);
    int current = tour[0];
    double total = 0.0;

    for (int i = 1; i < n; i++) {
        double best_d = DBL_MAX;
        int best_j = -1;

        const double *row = &costs[current * n];

        for (int j = i; j < n; j++) {
            const int cand = tour[j];
            const double d = row[cand];

            if (d < best_d) {
                best_d = d;
                best_j = j;
            }
        }

        if (best_j == -1) {
            if_verbose(VERBOSE_INFO, "[ERROR] NN: no candidate at step %d\n", i);
            return -1;
        }

        total += best_d;
        swap_int(tour + i, tour + best_j);
        current = tour[i];
    }

    /* Closing arc is relevant for debugging differences in heuristic behavior */
    total += costs[current * n + tour[0]];

    tour[n] = tour[0];
    *cost = total;

    if_verbose(VERBOSE_DEBUG, "\tNN: tour built, cost=%.6f\n", total);

    return 0;
}


/**
 * @brief Generates a tour using a modified nearest neighbor approach with probabilistic selection.
 */
int grasp_nearest_neighbor_tour(const int starting_node,
                                int *tour,
                                const int n,
                                const double *costs,
                                double *cost,
                                const int rcl_size,
                                const double probability) {
    /* Report invalid parameters explicitly */
    if (starting_node < 0 || starting_node >= n) {
        if_verbose(VERBOSE_INFO, "[ERROR] GRASP-NN: starting node %d out of bounds [0,%d)\n", starting_node, n);
        return -1;
    }
    if (rcl_size < 1) {
        if_verbose(VERBOSE_INFO, "[ERROR] GRASP-NN: RCL size %d is invalid\n", rcl_size);
        return -1;
    }

    if_verbose(VERBOSE_DEBUG, "\tGRASP-NN: start=%d, RCL=%d, prob=%.3f\n", starting_node, rcl_size, probability);

    int *rcl_nodes = malloc(rcl_size * sizeof(int));
    double *rcl_costs = malloc(rcl_size * sizeof(double));
    check_alloc(rcl_nodes);
    check_alloc(rcl_costs);

    for (int i = 0; i < n; i++)
        tour[i] = i;

    swap_int(tour, tour + starting_node);

    int current = tour[0];
    double total = 0.0;

    for (int i = 1; i < n; i++) {
        /* RCL reset avoids leftover garbage between iterations */
        for (int k = 0; k < rcl_size; k++) {
            rcl_nodes[k] = -1;
            rcl_costs[k] = DBL_MAX;
        }

        const double *row = &costs[current * n];
        int candidates = 0;

        for (int j = i; j < n; j++) {
            const int cand = tour[j];
            const double d = row[cand];

            /* Keep RCL sorted; smallest elements represent strongest greedy bias */
            if (d < rcl_costs[rcl_size - 1]) {
                for (int k = 0; k < rcl_size; k++) {
                    if (d < rcl_costs[k]) {
                        for (int shift = rcl_size - 1; shift > k; shift--) {
                            rcl_costs[shift] = rcl_costs[shift - 1];
                            rcl_nodes[shift] = rcl_nodes[shift - 1];
                        }
                        rcl_costs[k] = d;
                        rcl_nodes[k] = j;
                        break;
                    }
                }
            }
        }

        /* Count how many slots of the RCL are valid */
        for (int k = 0; k < rcl_size; k++) {
            if (rcl_nodes[k] != -1) candidates++;
            else break;
        }

        if (candidates == 0) {
            if_verbose(VERBOSE_INFO, "[ERROR] GRASP-NN: empty RCL at step %d\n", i);
            free(rcl_nodes);
            free(rcl_costs);
            return -1;
        }

        int chosen;
        double added;

        /* Random choice increases diversification under high competition */
        if (normalized_rand() <= probability) {
            const int r = (int) (normalized_rand() * candidates);
            chosen = rcl_nodes[r];
            added = rcl_costs[r];
        } else {
            chosen = rcl_nodes[0];
            added = rcl_costs[0];
        }

        total += added;
        swap_int(tour + i, tour + chosen);
        current = tour[i];
    }

    /* Closing arc is recorded for parity with the deterministic NN */
    total += costs[current * n + tour[0]];

    tour[n] = tour[0];
    *cost = total;

    if_verbose(VERBOSE_DEBUG, "\tGRASP-NN: tour complete, cost=%.6f\n", total);

    free(rcl_nodes);
    free(rcl_costs);
    return 0;
}


int grasp_nearest_neighbor_tour_threshold(const int starting_node,
                                          int *tour,
                                          const int number_of_nodes,
                                          const double *edge_cost_array,
                                          double *cost,
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
               "\tGRASP-Threshold: Building tour from node %d (alpha=%.2f)...\n",
               starting_node, alpha);

    // initialize tour permutation
    for (int i = 0; i < number_of_nodes; i++)
        tour[i] = i;
    swap_int(tour, tour + starting_node);

    int visited_count = 1;
    int current_node = tour[0];
    tour[number_of_nodes] = tour[0];

    // RCL allocation
    int *rcl = malloc(number_of_nodes * sizeof(int));
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
            if_verbose(VERBOSE_INFO, "[ERROR] GRASP-Threshold: No candidates found.\n");
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
        const int chosen_index = rcl[(int) (normalized_rand() * rcl_size)];

        if_verbose(VERBOSE_DEBUG, "\tGRASP-Threshold: Chose node %d (RCL size %d).\n", tour[chosen_index], rcl_size);

        // commit choice
        swap_int(tour + visited_count, tour + chosen_index);
        current_node = tour[visited_count];
        visited_count++;
    }

    free(rcl);
    *cost = calculate_tour_cost(tour, number_of_nodes, edge_cost_array);

    if_verbose(VERBOSE_DEBUG, "\tGRASP-Threshold: Built tour. Cost: %lf\n", *cost);

    return 0;
}
