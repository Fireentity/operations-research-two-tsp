#ifndef TSP_PARSER_H
#define TSP_PARSER_H

#include "tsp_instance.h"

typedef enum {
    PARSE_OK = 0,
    PARSE_ERR_OPEN = -1,
    PARSE_ERR_FORMAT = -2,
    PARSE_ERR_MEMORY = -3,
    PARSE_ERR_EXT = -4,
    PARSE_ERR_DIM = -5,
    PARSE_ERR_INV = -6,
    PARSE_ERR_OVERFLOW = -7
} TspParserStatus;

int tsp_parser_load_instance(const char *path, Node **out_nodes, int *out_n);
int tsp_parser_load_solution(const char *path, int n, int *tour_buf, double *out_cost);

#endif
