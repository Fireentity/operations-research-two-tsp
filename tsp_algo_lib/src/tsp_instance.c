#include <tsp_instance.h>
#include <c_util.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <tsp_math_util.h>

struct TspInstanceState
{
    double* const edge_cost_array; // Precomputed array of edge costs
    const int number_of_nodes;     // Total number of nodes
    const Node* const nodes;       // Array of node coordinates
};

static int get_number_of_nodes(const TspInstance* self) { return self->state->number_of_nodes; }

static const double* get_edge_cost_array(const TspInstance* self) { return self->state->edge_cost_array; }

static const Node* get_nodes(const TspInstance* self) { return self->state->nodes; }

static Node* init_nodes(const int number_of_nodes, const TspGenerationArea generation_area)
{
    Node* const nodes = malloc(number_of_nodes * sizeof(Node));
    check_alloc(nodes); // Ensure nodes allocation succeeded
    for (int i = 0; i < number_of_nodes; i++)
    {
        // Randomly initialize node coordinates within the generation area
        nodes[i].x = generation_area.x_square + normalized_rand() * generation_area.square_side;
        nodes[i].y = generation_area.y_square + normalized_rand() * generation_area.square_side;
    }
    return nodes;
}

static void free_this(TspInstance* solution)
{
    // Return early if any required pointer is missing
    if (!solution || !solution->state->edge_cost_array || !solution->state->nodes)
    {
        return;
    }
    free(solution->state->edge_cost_array);
    free((void*)solution->state->nodes);
    free(solution);
}

const TspInstance* init_random_tsp_instance(const int number_of_nodes,
                                            const int seed,
                                            const TspGenerationArea generation_area)
{
    srand(seed); // Seed the random number generator
    Node* nodes = init_nodes(number_of_nodes, generation_area);
    double* edge_cost_array = init_edge_cost_array(nodes, number_of_nodes);

    const TspInstanceState state = {
            .number_of_nodes = number_of_nodes,
            .nodes = nodes,
            .edge_cost_array = edge_cost_array,
    };

    const TspInstance instance = {
            // Use custom allocation that copies the stack object
            .state = malloc_from_stack(&state, sizeof(state)),
            .get_edge_cost_array = get_edge_cost_array,
            .get_number_of_nodes = get_number_of_nodes,
            .get_nodes = get_nodes,
    };
    return malloc_from_stack(&instance, sizeof(instance));
}
