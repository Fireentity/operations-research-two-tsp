#include "constructive.h"
#include "constants.h"
#include "c_util.h"
#include "logger.h"
#include "tsp_math.h"
#include <tgmath.h>
#include <float.h>
#include <stdlib.h>
#include <string.h>

/* --- Nearest Neighbor Implementation --- */
int nearest_neighbor_tour(const int starting_node,
                          int *tour,
                          const int number_of_nodes,
                          const double *edge_cost_array,
                          double *cost) {
    if (starting_node < 0 || starting_node >= number_of_nodes) {
        if_verbose(VERBOSE_INFO,
                   "[ERROR] NN: starting node %d out of bounds [0,%d)\n",
                   starting_node, number_of_nodes);
        return -1;
    }

    if_verbose(VERBOSE_DEBUG, "\tNN: constructing tour from start %d\n", starting_node);

    for (int i = 0; i < number_of_nodes; i++)
        tour[i] = i;

    swap_int(tour, tour + starting_node);

    int current_node = tour[0];
    double total_cost = 0.0;

    for (int i = 1; i < number_of_nodes; i++) {
        double best_dist = DBL_MAX;
        int best_idx = -1;

        const double *row_costs = &edge_cost_array[current_node * number_of_nodes];

        for (int j = i; j < number_of_nodes; j++) {
            const int candidate = tour[j];
            const double d = row_costs[candidate];

            if (d < best_dist) {
                best_dist = d;
                best_idx = j;
            }
        }

        if (best_idx == -1) {
            if_verbose(VERBOSE_INFO, "[ERROR] NN: no candidate at step %d\n", i);
            return -1;
        }

        total_cost += best_dist;
        swap_int(tour + i, tour + best_idx);
        current_node = tour[i];
    }

    total_cost += edge_cost_array[current_node * number_of_nodes + tour[0]];

    tour[number_of_nodes] = tour[0];
    *cost = total_cost;

    if_verbose(VERBOSE_DEBUG, "\tNN: tour built, cost=%.6f\n", total_cost);

    return 0;
}

/* --- GRASP Nearest Neighbor Implementation --- */
int grasp_nearest_neighbor_tour(const int starting_node,
                                int *tour,
                                const int number_of_nodes,
                                const double *edge_cost_array,
                                double *cost,
                                const int rcl_size,
                                const double probability) {
    if (starting_node < 0 || starting_node >= number_of_nodes) {
        if_verbose(VERBOSE_INFO,
                   "[ERROR] GRASP-NN: starting node %d out of bounds [0,%d)\n",
                   starting_node, number_of_nodes);
        return -1;
    }
    if (rcl_size < 1) {
        if_verbose(VERBOSE_INFO,
                   "[ERROR] GRASP-NN: RCL size %d is invalid\n",
                   rcl_size);
        return -1;
    }

    if_verbose(VERBOSE_DEBUG,
               "\tGRASP-NN: start=%d, RCL=%d, prob=%.3f\n",
               starting_node, rcl_size, probability);

    int *rcl_nodes = malloc(rcl_size * sizeof(int));
    double *rcl_costs = malloc(rcl_size * sizeof(double));
    check_alloc(rcl_nodes);
    check_alloc(rcl_costs);

    for (int i = 0; i < number_of_nodes; i++)
        tour[i] = i;

    swap_int(tour, tour + starting_node);

    int current_node = tour[0];
    double total_cost = 0.0;

    for (int i = 1; i < number_of_nodes; i++) {
        for (int k = 0; k < rcl_size; k++) {
            rcl_nodes[k] = -1;
            rcl_costs[k] = DBL_MAX;
        }

        const double *row_costs = &edge_cost_array[current_node * number_of_nodes];

        for (int j = i; j < number_of_nodes; j++) {
            const int candidate = tour[j];
            const double d = row_costs[candidate];

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

        int candidates_found = 0;
        for (int k = 0; k < rcl_size; k++) {
            if (rcl_nodes[k] != -1) candidates_found++;
            else break;
        }

        if (candidates_found == 0) {
            if_verbose(VERBOSE_INFO,
                       "[ERROR] GRASP-NN: empty RCL at step %d\n", i);
            free(rcl_nodes);
            free(rcl_costs);
            return -1;
        }

        int chosen_tour_index;
        double added_cost;

        if (normalized_rand() <= probability) {
            const int r_idx = (int) (normalized_rand() * candidates_found);
            chosen_tour_index = rcl_nodes[r_idx];
            added_cost = rcl_costs[r_idx];
        } else {
            chosen_tour_index = rcl_nodes[0];
            added_cost = rcl_costs[0];
        }

        total_cost += added_cost;
        swap_int(tour + i, tour + chosen_tour_index);
        current_node = tour[i];
    }

    total_cost += edge_cost_array[current_node * number_of_nodes + tour[0]];
    tour[number_of_nodes] = tour[0];
    *cost = total_cost;

    if_verbose(VERBOSE_DEBUG, "\tGRASP-NN: tour complete, cost=%.6f\n", total_cost);

    free(rcl_nodes);
    free(rcl_costs);
    return 0;
}

/* --- GRASP Threshold Implementation (unchanged style) --- */
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

    for (int i = 0; i < number_of_nodes; i++)
        tour[i] = i;
    swap_int(tour, tour + starting_node);

    int visited_count = 1;
    int current_node = tour[0];
    tour[number_of_nodes] = tour[0];

    int *rcl = malloc(number_of_nodes * sizeof(int));
    check_alloc(rcl);

    const double effective_alpha = fmax(0.0, fmin(1.0, alpha));

    while (visited_count < number_of_nodes) {
        double min_cost = DBL_MAX;
        double max_cost = -DBL_MAX;

        for (int i = visited_count; i < number_of_nodes; i++) {
            const int candidate = tour[i];
            const double dist =
                    edge_cost_array[current_node * number_of_nodes + candidate];
            if (dist < min_cost) min_cost = dist;
            if (dist > max_cost) max_cost = dist;
        }

        if (min_cost == DBL_MAX) {
            if_verbose(VERBOSE_INFO,
                       "[ERROR] GRASP-Threshold: No candidates found.\n");
            free(rcl);
            return -1;
        }

        const double threshold =
                min_cost + effective_alpha * (max_cost - min_cost);

        int rcl_size = 0;
        for (int i = visited_count; i < number_of_nodes; i++) {
            const int candidate = tour[i];
            const double dist =
                    edge_cost_array[current_node * number_of_nodes + candidate];
            if (dist <= threshold + EPSILON)
                rcl[rcl_size++] = i;
        }

        const int chosen_index = rcl[(int) (normalized_rand() * rcl_size)];

        if_verbose(VERBOSE_DEBUG,
                   "\tGRASP-Threshold: Chose node %d (RCL size %d).\n",
                   tour[chosen_index], rcl_size);

        swap_int(tour + visited_count, tour + chosen_index);
        current_node = tour[visited_count];
        visited_count++;
    }

    free(rcl);
    *cost = calculate_tour_cost(tour, number_of_nodes, edge_cost_array);

    if_verbose(VERBOSE_DEBUG,
               "\tGRASP-Threshold: Built tour. Cost: %lf\n", *cost);

    return 0;
}

/* --- Extra Mileage / Cheapest Insertion Logic --- */

int extra_mileage_complete_tour(int *tour,
                                int current_count,
                                const int n,
                                const double *costs,
                                int *visited) {
    while (current_count < n) {
        double best_delta = DBL_MAX;
        int best_node = -1;
        int best_pos = -1;

        for (int h = 0; h < n; h++) {
            if (visited[h]) continue;

            for (int p = 0; p < current_count; p++) {
                const int i = tour[p];
                const int j = tour[(p + 1) % current_count];

                const double delta =
                        costs[i * n + h] +
                        costs[h * n + j] -
                        costs[i * n + j];

                if (delta < best_delta) {
                    best_delta = delta;
                    best_node = h;
                    best_pos = p;
                }
            }
        }

        if (best_node == -1) {
            if_verbose(VERBOSE_INFO,
                       "[ERROR] Extra-Mileage: no feasible insertion found.\n");
            return -1;
        }

        if_verbose(VERBOSE_DEBUG,
                   "\tExtra-Mileage: inserting node %d at pos %d (delta=%.6f)\n",
                   best_node, best_pos, best_delta);

        const int move_count = current_count - (best_pos + 1);
        if (move_count > 0) {
            memmove(&tour[best_pos + 2],
                    &tour[best_pos + 1],
                    move_count * sizeof(int));
        }

        tour[best_pos + 1] = best_node;
        visited[best_node] = 1;
        current_count++;
    }

    tour[n] = tour[0];
    return 0;
}

int extra_mileage_tour(int *tour,
                       const int n,
                       const double *costs,
                       double *cost) {
    if (n < 2) {
        if_verbose(VERBOSE_INFO,
                   "[ERROR] Extra-Mileage: n=%d is too small.\n", n);
        return -1;
    }

    if_verbose(VERBOSE_DEBUG,
               "\tExtra-Mileage: starting construction for n=%d\n", n);

    int node_a = 0, node_b = 1;
    double max_dist = -DBL_MAX;

    for (int i = 0; i < n; i++) {
        const double *row = &costs[i * n];
        for (int j = i + 1; j < n; j++) {
            const double d = row[j];
            if (d > max_dist) {
                max_dist = d;
                node_a = i;
                node_b = j;
            }
        }
    }

    if_verbose(VERBOSE_DEBUG,
               "\tExtra-Mileage: diameter nodes = (%d,%d), dist=%.6f\n",
               node_a, node_b, max_dist);

    int *visited = calloc(n, sizeof(int));
    check_alloc(visited);

    tour[0] = node_a;
    tour[1] = node_b;
    visited[node_a] = 1;
    visited[node_b] = 1;

    if (extra_mileage_complete_tour(tour, 2, n, costs, visited) != 0) {
        free(visited);
        return -1;
    }

    *cost = calculate_tour_cost(tour, n, costs);

    if_verbose(VERBOSE_DEBUG,
               "\tExtra-Mileage: tour completed, cost=%.6f\n", *cost);

    free(visited);
    return 0;
}
