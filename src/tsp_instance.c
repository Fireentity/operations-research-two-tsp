#include "tsp_instance.h"
#include "c_util.h"
#include "math_util.h"

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
            const unsigned long dx = instance->nodes[i].x - instance->nodes[j].x;
            const unsigned long dy = instance->nodes[i].y - instance->nodes[j].y;
            instance->edge_cost_array[i * number_of_nodes + j] = sqrt((double) (dx * dx + dy * dy));
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
        //TODO Con questo metodo i nodi non possono avere la virgola pensare di passare a nodi non interi
        nodes[i].x = generation_area.x_square + rand() % (generation_area.square_side + 1);
        nodes[i].y = generation_area.y_square + rand() % (generation_area.square_side + 1);
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
        printf("Node[%zu]: \tx = %ld,\ty = %ld\n", i, nodes[i].x, nodes[i].y);

    printf("Edge Cost Matrix:\n");
    for (unsigned long i = 0; i < number_of_nodes; i++) {
        for (unsigned long j = 0; j < number_of_nodes; j++) {
            printf("%.2f\t ", instance_ptr->edge_cost_array[i * number_of_nodes + j]);
        }
        printf("\n");
    }

    return instance_ptr;
}

void plot_tour(const unsigned long *tour, const TspInstance *instance, const char *output_name) {
    if (!output_name) {
        output_name = "tsp_solution.png"; // Nome di default
    }

    FILE *gp = popen("gnuplot", "w");
    check_popen(gp);

    fprintf(gp, "set terminal png size 800,600\n");

    Bounds bounds = calculate_plot_bounds(tour, instance);

    fprintf(gp, "set xrange [%lf:%lf]\n", bounds.min_x, bounds.max_x);
    fprintf(gp, "set yrange [%lf:%lf]\n", bounds.min_y, bounds.max_y);

    fprintf(gp, "set output '%s'\n", output_name);
    fprintf(gp, "plot '-' with linespoints lw 2 pt 7 notitle\n");

    for (unsigned long i = 0; i <= instance->number_of_nodes; i++) {
        Node const node = instance->nodes[tour[i]];
        fprintf(gp, "%lf %lf\n", (double) node.x, (double) node.y);
    }

    fprintf(gp, "e\n");
    fflush(gp);
    check_pclose(pclose(gp));
}

const Node *get_nodes(const TspInstance *instance) {
    return instance->nodes;
}

void solve_instance(TspSolver solver, unsigned long *tour, double *cost, const TspInstance *instance) {
    solver(tour, instance->number_of_nodes, instance->edge_cost_array, cost);
}


