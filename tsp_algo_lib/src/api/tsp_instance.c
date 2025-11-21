#include "tsp_instance.h"
#include "tsp_parser.h"
#include "c_util.h"
#include "tsp_math_util.h"
#include "logger.h"
#include <stdlib.h>
#include <string.h>

struct TspInstance {
    int number_of_nodes;
    Node *nodes;
    double *edge_cost_array;
};

static TspInstance *instance_create_from_nodes(Node *nodes, const int n) {
    double *costs = init_edge_cost_array(nodes, n);

    TspInstance *inst = malloc(sizeof(TspInstance));
    check_alloc(inst);

    inst->number_of_nodes = n;
    inst->nodes = nodes;
    inst->edge_cost_array = costs;

    return inst;
}

TspInstance *tsp_instance_create_random(const int number_of_nodes, const int seed, const TspGenerationArea area) {
    srand(seed);

    Node *nodes = malloc(number_of_nodes * sizeof(Node));
    check_alloc(nodes);

    for (int i = 0; i < number_of_nodes; i++) {
        nodes[i].x = area.x_square + normalized_rand() * area.square_side;
        nodes[i].y = area.y_square + normalized_rand() * area.square_side;
    }

    return instance_create_from_nodes(nodes, number_of_nodes);
}

TspInstance *tsp_instance_create(const Node *nodes, const int number_of_nodes) {
    Node *copy = malloc(number_of_nodes * sizeof(Node));
    check_alloc(copy);
    memcpy(copy, nodes, number_of_nodes * sizeof(Node));
    return instance_create_from_nodes(copy, number_of_nodes);
}

TspError tsp_instance_load_from_file(const char *path, TspInstance **out_instance) {
    Node *nodes = NULL;
    int n = 0;

    const TspError err = tsp_parse_by_extension(path, &nodes, &n);
    if (err != TSP_OK) return err;

    *out_instance = instance_create_from_nodes(nodes, n);
    return TSP_OK;
}

void tsp_instance_free(TspInstance *instance) {
    if (!instance) return;
    free(instance->nodes);
    free(instance->edge_cost_array);
    free(instance);
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

const char *tsp_error_to_string(const TspError err) {
    switch (err) {
        case TSP_OK: return "Success";
        case TSP_ERR_FILE_OPEN: return "Unable to open file";
        case TSP_ERR_PARSE_HEADER: return "Malformed TSPLIB header";
        case TSP_ERR_PARSE_NODES: return "Invalid node coordinates";
        case TSP_ERR_MEMORY: return "Memory allocation failed";
        case TSP_ERR_INVALID_EXT: return "Unsupported file extension";
        case TSP_ERR_UNKNOWN_FORMAT: return "Unknown format";
        default: return "Unknown error";
    }
}
