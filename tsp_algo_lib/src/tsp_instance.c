#include "tsp_instance.h"
#include "c_util.h"
#include "math_util.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "../../common/include/c_util.h"

/**
 * @struct TspInstance
 * @brief Represents an instance of the Traveling Salesman Problem (TSP).
 *
 * This structure holds all the necessary data for solving a TSP instance.
 *
 * @var TspInstance::cost
 * The total cost of the current tour.
 *
 * @var TspInstance::number_of_nodes
 * The total number of nodes in the problem.
 *
 * @var TspInstance::nodes
 * Pointer to an array of nodes, each containing coordinates.
 *
 * @var TspInstance::solution
 * Pointer to an array of indices representing the tour.
 * The first node is duplicated at the end of the array to simplify
 * the evaluation of the last edge connecting the last node back to the start.
 */
struct TspInstance {
    double *const edge_cost_array;
    const unsigned long number_of_nodes;
    const Node *const nodes;
};

unsigned long get_number_of_nodes(const TspInstance *instance) {
    return instance->number_of_nodes;
}


const double *get_edge_cost_array(const TspInstance *instance) {
    return instance->edge_cost_array;
}

void fill_edge_cost_matrix(const TspInstance *instance) {
    const unsigned long number_of_nodes = instance->number_of_nodes;
    for (unsigned long i = 0; i < number_of_nodes; i++) {
        for (unsigned long j = 0; j < number_of_nodes; j++) {
            const double dx = instance->nodes[i].x - instance->nodes[j].x;
            const double dy = instance->nodes[i].y - instance->nodes[j].y;
            instance->edge_cost_array[i * number_of_nodes + j] = sqrt((dx * dx + dy * dy));
        }
    }
}

const TspInstance *init_random_tsp_instance(const unsigned long number_of_nodes,
                                            const int seed,
                                            const Rectangle generation_area) {
    srand(seed);

    Node *nodes = malloc(number_of_nodes * sizeof(Node));
    check_alloc(nodes);
    for (unsigned long i = 0; i < number_of_nodes; i++) {
        nodes[i].x = generation_area.x_square + random01() * generation_area.square_side;
        nodes[i].y = generation_area.y_square + random01() * generation_area.square_side;
    }

    double *edge_cost_array = calloc(number_of_nodes * number_of_nodes, sizeof(double));
    check_alloc(edge_cost_array);

    TspInstance *instance_ptr = malloc(sizeof(TspInstance));
    check_alloc(instance_ptr);
    const TspInstance instance = {
            .number_of_nodes = number_of_nodes,
            .nodes = nodes,
            .edge_cost_array = edge_cost_array,
    };
    memcpy(instance_ptr, &instance, sizeof(instance));

    fill_edge_cost_matrix(instance_ptr);

    for (unsigned long i = 0; i < number_of_nodes; i++)
        printf("Node[%zu]: \tx = %lf,\ty = %lf\n", i, nodes[i].x, nodes[i].y);

    printf("Edge Cost Matrix:\n");
    for (unsigned long i = 0; i < number_of_nodes; i++) {
        for (unsigned long j = 0; j < number_of_nodes; j++) {
            printf("%.2f\t ", instance_ptr->edge_cost_array[i * number_of_nodes + j]);
        }
        printf("\n");
    }

    return instance_ptr;
}

const Node *get_nodes(const TspInstance *instance) {
    return instance->nodes;
}



