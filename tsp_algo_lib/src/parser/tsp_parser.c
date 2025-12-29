#include "tsp_parser.h"
#include "tsp_parser_instance.h"
#include "tsp_parser_solution.h"
#include <string.h>
#include <strings.h>

typedef struct {
    const char *ext;

    int (*fn)(const char *, Node **, int *);
} InstEntry;

typedef struct {
    const char *ext;

    int (*fn)(const char *, int, int *, double *);
} SolEntry;

static const InstEntry inst_registry[] = {
    {".tsp", parse_instance_tsplib},
    {NULL, NULL}
};

static const SolEntry sol_registry[] = {
    {".tspsol", parse_solution_v1},
    {".sol", parse_solution_v1},
    {NULL, NULL}
};

static const char *get_ext(const char *path) {
    const char *ext = strrchr(path, '.');
    return ext ? ext : "";
}

TspParserStatus tsp_parser_load_instance(const char *path, Node **out_nodes, int *out_n) {
    const char *ext = get_ext(path);
    for (int i = 0; inst_registry[i].ext; i++)
        if (strcasecmp(ext, inst_registry[i].ext) == 0)
            return inst_registry[i].fn(path, out_nodes, out_n);
    return PARSE_ERR_EXT;
}

TspParserStatus tsp_parser_load_solution(const char *path, int n, int *tour_buf, double *out_cost) {
    const char *ext = get_ext(path);
    for (int i = 0; sol_registry[i].ext; i++)
        if (strcasecmp(ext, sol_registry[i].ext) == 0)
            return sol_registry[i].fn(path, n, tour_buf, out_cost);
    return PARSE_ERR_EXT;
}
