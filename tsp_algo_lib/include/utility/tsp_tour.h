#ifndef TSP_TOUR_H
#define TSP_TOUR_H

double compute_n_opt_cost(int number_of_segments,
                          int tour[],
                          const int *edges_to_remove,
                          const double edge_cost_array[],
                          int number_of_nodes);

void compute_n_opt_move(int number_of_edges_to_remove,
                        int tour[],
                        const int *edges_to_remove,
                        int number_of_nodes);
#endif //TSP_TOUR_H
