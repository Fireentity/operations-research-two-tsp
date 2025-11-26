#ifndef CONSTRUCTIVE_H
#define CONSTRUCTIVE_H

int nearest_neighbor_tour(int starting_node,
                          int *tour,
                          int number_of_nodes,
                          const double *edge_cost_array,
                          double *cost);

int grasp_nearest_neighbor_tour(int starting_node,
                                int *tour,
                                int number_of_nodes,
                                const double *edge_cost_array,
                                double *cost,
                                int rcl_size,
                                double probability);

int grasp_nearest_neighbor_tour_threshold(int starting_node,
                                          int *tour,
                                          int number_of_nodes,
                                          const double *edge_cost_array,
                                          double *cost,
                                          double alpha);

#endif //CONSTRUCTIVE_H