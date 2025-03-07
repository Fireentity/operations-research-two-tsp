#include "tsp_instance.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

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
struct TspInstance
{
    double* edge_cost_matrix;
    long number_of_nodes;
    Node* nodes;
};

long get_number_of_nodes(const TspInstance* instance)
{
    return instance->number_of_nodes;
}


const double* get_edge_cost_array(const TspInstance* instance)
{
    return instance->edge_cost_matrix;
}

void fill_edge_cost_matrix(const TspInstance* instance)
{
    const long number_of_nodes = instance->number_of_nodes;
    for (int i = 0; i < number_of_nodes; i++)
    {
        for (int j = 0; j < number_of_nodes; j++)
        {
            const long dx = instance->nodes[i].x - instance->nodes[j].x;
            const long dy = instance->nodes[i].y - instance->nodes[j].y;
            instance->edge_cost_matrix[i * number_of_nodes + j] = sqrt((double)(dx * dx + dy * dy));
        }
    }
}

TspInstance* initialize_random_tsp_instance(const TspParams* params)
{
    const long number_of_nodes = params->number_of_nodes;
    srand(params->seed);
    Node* nodes = malloc(number_of_nodes * sizeof(Node));
    if (!nodes)
    {
        fprintf(stderr, "Allocation error\n");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < number_of_nodes; i++)
    {
        nodes[i].x = params->generation_area.x_square + rand() % (params->generation_area.square_side + 1);
        nodes[i].y = params->generation_area.y_square + rand() % (params->generation_area.square_side + 1);
    }
    TspInstance* instance = malloc(sizeof(TspInstance));
    instance->number_of_nodes = number_of_nodes;
    instance->nodes = nodes;
    instance->edge_cost_matrix = calloc(number_of_nodes * number_of_nodes, sizeof(double));
    fill_edge_cost_matrix(instance);
    return instance;
}
