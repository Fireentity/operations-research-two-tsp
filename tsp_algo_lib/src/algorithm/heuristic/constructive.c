#include "constructive.h"
#include "constants.h"
#include "c_util.h"
#include "logger.h"
#include "tsp_math.h"
#include <tgmath.h>
#include <float.h>
#include <stdlib.h>
#include <string.h>

#include "random.h"

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
                                const double probability,
                                RandomState *rng) {
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

    int *rcl_nodes = tsp_malloc(rcl_size * sizeof(int));
    double *rcl_costs = tsp_malloc(rcl_size * sizeof(double));


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
            tsp_free(rcl_nodes);
            tsp_free(rcl_costs);
            return -1;
        }

        int chosen_tour_index;
        double added_cost;

        if (random_double(rng) <= probability) {
            const int r_idx = random_int(rng, 0, candidates_found - 1);
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

    tsp_free(rcl_nodes);
    tsp_free(rcl_costs);
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

    int *visited = tsp_calloc(n, sizeof(int));


    tour[0] = node_a;
    tour[1] = node_b;
    visited[node_a] = 1;
    visited[node_b] = 1;

    if (extra_mileage_complete_tour(tour, 2, n, costs, visited) != 0) {
        tsp_free(visited);
        return -1;
    }

    *cost = calculate_tour_cost(tour, n, costs);

    if_verbose(VERBOSE_DEBUG,
               "\tExtra-Mileage: tour completed, cost=%.6f\n", *cost);

    tsp_free(visited);
    return 0;
}
