#include "algorithms.h"
#include <tsp_math_util.h>
#include <float.h>
#include "c_util.h"


inline double two_opt(int* tour,
                      const int number_of_nodes,
                      const double* edge_cost_array,
                      const TimeLimiter* time_limiter,
                      const double epsilon)
{
    double cost_improvement = 0;
    // Si assume che tour abbia dimensione number_of_nodes+1
    int i = 1;
    while (i < number_of_nodes - 1)
    {
        if (time_limiter->is_time_over(time_limiter))
            return cost_improvement;

        bool improvement_found = false;
        // j rappresenta l'indice di fine del segmento da invertire
        for (int j = i + 1; j < number_of_nodes; j++)
        {
            // Evita la mossa che invertirebbe l'intero tour
            if (i == 1 && j == number_of_nodes - 1)
                continue;

            const int a = tour[i - 1];
            const int b = tour[i];
            const int c = tour[j];
            const int d = tour[j + 1]; // tour è di dimensione number_of_nodes+1

            // Calcola il delta in modo ottimizzato:
            // delta = (costo arco a-c + costo arco b-d) - (costo arco a-b + costo arco c-d)
            const double delta = (edge_cost_array[a * number_of_nodes + c] +
                    edge_cost_array[b * number_of_nodes + d]) -
                (edge_cost_array[a * number_of_nodes + b] +
                    edge_cost_array[c * number_of_nodes + d]);

            // Se la mossa porta a un miglioramento
            if (delta < -epsilon)
            {
                cost_improvement += delta;
                // Inverte il segmento tra gli indici i e j utilizzando la macro definita
                reverse_array_int(tour, i, j);
                improvement_found = true;
                break;
            }
        }

        // Se è stato applicato un miglioramento, si riavvia da i = 1
        i = improvement_found ? 1 : i + 1;
    }

    return cost_improvement;
}

void nearest_neighbor_tour(const int starting_node,
                           int* tour,
                           const int number_of_nodes,
                           const double* edge_cost_array,
                           double* cost)
{
    if (starting_node > number_of_nodes)
    {
        printf("The starting node (%d) cannot be greater than the number of nodes (%d)",
               starting_node, number_of_nodes);
        exit(EXIT_FAILURE);
    }


    swap_int(tour, 0, starting_node);

    int visited_count = 1;
    int current_node = tour[0];

    // Closing the tour
    tour[number_of_nodes] = tour[0];

    while (visited_count < number_of_nodes)
    {
        double best_cost = DBL_MAX;
        int best_index = visited_count;

        // Find the nearest unvisited node
        for (int i = visited_count; i < number_of_nodes; i++)
        {
            const double cost_candidate = edge_cost_array[current_node * number_of_nodes + tour[i]];
            if (cost_candidate < best_cost)
            {
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
 * nearest_neighbor_tour with the modified step:
 * Instead of always picking the nearest unvisited node, pick among the 2nd, 3rd, or 4th
 * nearest nodes with probabilities p2, p3, and p4 (if possible).
 */
void grasp_nearest_neighbor_tour(const int starting_node,
                                 int* tour,
                                 const int number_of_nodes,
                                 const double* edge_cost_array,
                                 double* cost,
                                 const double p1,
                                 const double p2,
                                 const double p3)
{
    if (starting_node >= number_of_nodes)
    {
        printf("The starting node (%d) cannot be >= the number of nodes (%d)\n",
               starting_node, number_of_nodes);
        exit(EXIT_FAILURE);
    }

    swap_int(tour, 0, starting_node);

    // We will keep track of how many nodes have been visited in the tour
    int visited_count = 1;
    int current_node = tour[0];

    // Close the tour (make it a cycle)
    tour[number_of_nodes] = tour[0];

    // Continue until all nodes have been visited
    while (visited_count < number_of_nodes)
    {
        // Find the 4 closest unvisited nodes

        // We'll keep track of the indices of the 4 closest nodes (in ascending order)
        // min_index[k] will hold the index in 'tour' of the k-th closest candidate
        // min_dist[k] will hold the corresponding distance
        int min_index[4] = {-1, -1, -1, -1};
        double min_dist[4] = {DBL_MAX, DBL_MAX, DBL_MAX, DBL_MAX};

        // Loop through unvisited nodes to find up to the 4 smallest distances
        for (int i = visited_count; i < number_of_nodes; i++)
        {
            const int candidate = tour[i];
            const double dist = edge_cost_array[current_node * number_of_nodes + candidate];

            // Insert (candidate, dist) into the min_dist[] / min_index[] "top-4" list
            for (int k = 0; k < 4; k++)
            {
                if (dist < min_dist[k])
                {
                    // Shift the others down
                    for (int shift = 3; shift > k; shift--)
                    {
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
        for (int k = 0; k < 4; k++)
        {
            if (min_index[k] >= 0)
            {
                candidates_found++;
            }
            else
            {
                break;
            }
        }

        // Pick the first node with probability p1
        int chosen_index = -1;
        const double r = normalized_rand();

        if (r < p1 && candidates_found > 0)
        {
            chosen_index = min_index[0]; // First node is chosen with probability p1
        }
        else
        {
            // If the first node is not chosen, pick among the remaining candidates
            if (candidates_found >= 4)
            {
                // We have at least 4 unvisited nodes
                if (r < p2)
                {
                    chosen_index = min_index[1]; // 2nd nearest
                }
                else if (r < p2 + p3)
                {
                    chosen_index = min_index[2]; // 3rd nearest
                }
                else
                {
                    chosen_index = min_index[3]; // 4th nearest
                }
            }
            else
            {
                // Fewer than 4 nodes remain unvisited. Fallback logic:
                if (candidates_found == 3)
                {
                    const double p_sum = p2 + p3;
                    if (r < p2 / p_sum)
                    {
                        chosen_index = min_index[1];
                    }
                    else
                    {
                        chosen_index = min_index[2];
                    }
                }
                else if (candidates_found == 2)
                {
                    chosen_index = min_index[1];
                }
                else
                {
                    chosen_index = min_index[0]; // Only 1 candidate left
                }
            }
        }

        // "Visit" the chosen node by swapping it into the 'visited_count' position of the tour array.
        swap_int(tour, visited_count, chosen_index);

        // Advance
        current_node = tour[visited_count];
        visited_count++;
    }

    // Compute the total cost of the generated tour
    *cost = calculate_tour_cost(tour, number_of_nodes, edge_cost_array);
}

