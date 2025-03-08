#include "tsp_instance.h"
#include "c_util.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

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
    double* const edge_cost_array;
    const unsigned int number_of_nodes;
    const Node* const nodes;
};

long get_number_of_nodes(const TspInstance* instance)
{
    return instance->number_of_nodes;
}


const double* get_edge_cost_array(const TspInstance* instance)
{
    return instance->edge_cost_array;
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
            instance->edge_cost_array[i * number_of_nodes + j] = sqrt((double)(dx * dx + dy * dy));
        }
    }
}

const TspInstance* init_random_tsp_instance(const TspParams* params)
{
    const unsigned int number_of_nodes = params->number_of_nodes;
    srand(params->seed);

    Node* nodes = malloc(number_of_nodes * sizeof(Node));
    check_alloc(nodes);
    for (int i = 0; i < number_of_nodes; i++)
    {
        //TODO Con questo metodo i nodi non possono avere la virgola pensare di passare a nodi non interi
        nodes[i].x = params->generation_area.x_square + rand() % (params->generation_area.square_side + 1);
        nodes[i].y = params->generation_area.y_square + rand() % (params->generation_area.square_side + 1);
    }

    double* edge_cost_array = calloc(number_of_nodes * number_of_nodes, sizeof(double));
    check_alloc(edge_cost_array);

    TspInstance* instance_ptr = malloc(sizeof(TspInstance));
    check_alloc(instance_ptr);
    const TspInstance instance = {
        .number_of_nodes = number_of_nodes,
        .nodes = nodes,
        .edge_cost_array = edge_cost_array,
    };
    memcpy(instance_ptr, &instance, sizeof(instance));

    fill_edge_cost_matrix(instance_ptr);

    for (size_t i = 0; i < number_of_nodes; i++)
        printf("Node[%zu]: \tx = %ld,\ty = %ld\n", i, nodes[i].x, nodes[i].y);

    printf("Edge Cost Matrix:\n");
    for (long i = 0; i < number_of_nodes; i++)
    {
        for (long j = 0; j < number_of_nodes; j++)
        {
            printf("%.2f\t ", instance_ptr->edge_cost_array[i * number_of_nodes + j]);
        }
        printf("\n");
    }

    return instance_ptr;
}

void plot_tour(const int* tour, const TspInstance* instance)
{
    FILE* gp = popen("gnuplot", "w");
    fprintf(gp, "set terminal png size 800,600\n");
    fprintf(gp, "set output 'tsp_solution.png'\n");
    fprintf(gp, "plot '-' with linespoints lw 2 pt 7 notitle\n");
    for (long i = 0; i <= instance->number_of_nodes; i++)
    {
        Node const node = instance->nodes[tour[i]];
        fprintf(gp, "%lf %lf\n", (float)node.x, (float)node.y);
    }
    fprintf(gp, "e\n");
    pclose(gp);
}
