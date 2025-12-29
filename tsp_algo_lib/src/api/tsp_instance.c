#include "tsp_instance.h"
#include "tsp_parser.h"
#include "c_util.h"
#include "tsp_math.h"
#include "logger.h"
#include <stdlib.h>
#include <string.h>
#include "random.h"

struct TspInstance {
    int number_of_nodes;
    Node *nodes;
    double *edge_cost_array;
};

static TspInstance *instance_create_from_nodes(Node *nodes, const size_t n) {
    double *costs = init_edge_cost_array(nodes, n);

    TspInstance *inst = tsp_malloc(sizeof(TspInstance));


    inst->number_of_nodes = n;
    inst->nodes = nodes;
    inst->edge_cost_array = costs;

    return inst;
}

TspInstance *tsp_instance_create_random(const size_t number_of_nodes, const TspGenerationArea area) {
    Node *nodes = tsp_malloc(number_of_nodes * sizeof(Node));


    for (size_t i = 0; i < number_of_nodes; i++) {
        nodes[i].x = area.x_square + global_random_double() * area.square_side;
        nodes[i].y = area.y_square + global_random_double() * area.square_side;
    }

    return instance_create_from_nodes(nodes, number_of_nodes);
}

TspInstance *tsp_instance_create(const Node *nodes, const int number_of_nodes) {
    Node *copy = tsp_malloc(number_of_nodes * sizeof(Node));

    memcpy(copy, nodes, number_of_nodes * sizeof(Node));
    return instance_create_from_nodes(copy, number_of_nodes);
}

TspError tsp_instance_load_from_file(TspInstance **out_instance, const char *path) {
    *out_instance = NULL;
    Node *nodes = NULL;
    int n = 0;

    const TspParserStatus status = tsp_parser_load_instance(path, &nodes, &n);
    const TspError err = tsp_error_from_parser_status(status);
    if (err != TSP_OK) return err;

    *out_instance = instance_create_from_nodes(nodes, n);
    return TSP_OK;
}

void tsp_instance_destroy(TspInstance *instance) {
    if (!instance) return;
    if (instance->nodes)
        tsp_free(instance->nodes);
    if (instance->edge_cost_array)
        tsp_free(instance->edge_cost_array);
    tsp_free(instance);
}

int tsp_instance_get_num_nodes(const TspInstance *instance) {
    return instance ? instance->number_of_nodes : 0;
}

const Node *tsp_instance_get_nodes(const TspInstance *instance) {
    return instance ? instance->nodes : NULL;
}

const double *tsp_instance_get_cost_matrix(const TspInstance *instance) {
    return instance ? instance->edge_cost_array : NULL;
}
