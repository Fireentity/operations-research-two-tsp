#include "algorithms.h"
#include <tsp_math_util.h>
#include <constants.h>
#include <float.h>

#include "c_util.h"


inline double two_opt(int* tour,
                      const int number_of_nodes,
                      const double* edge_cost_array,
                      const TimeLimiter* time_limiter)
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
            if (delta < -EPSILON)
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
                                  int *tour,
                                  const int number_of_nodes,
                                  const double *edge_cost_array,
                                  double *cost) {
    if (starting_node > number_of_nodes) {
        printf("The starting node (%d) cannot be greater than the number of nodes (%d)",
               starting_node, number_of_nodes);
        exit(EXIT_FAILURE);
    }

    int visited = 1;

    // Start from the node in input
    swap_int(tour, 0, starting_node);
    int current = tour[0];

    // Closing the tour
    tour[number_of_nodes] = tour[0];

    while (visited < number_of_nodes) {
        double best_cost = DBL_MAX;
        int best_index = visited;

        // Find the nearest unvisited node
        for (int i = visited; i < number_of_nodes; i++) {
            const double cost_candidate = edge_cost_array[current * number_of_nodes + tour[i]];
            if (cost_candidate < best_cost) {
                best_cost = cost_candidate;
                best_index = i;
            }
        }
        // Move the best found node to the next position in the tour
        swap_int(tour, visited, best_index);
        current = tour[visited];
        visited++;
    }

    // Compute the total cost of the generated tour
    *cost = calculate_tour_cost(tour, number_of_nodes, edge_cost_array);
}



