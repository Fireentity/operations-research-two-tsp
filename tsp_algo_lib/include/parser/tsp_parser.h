#ifndef TSP_PARSER_H
#define TSP_PARSER_H
#include "tsp_error.h"

typedef struct Node Node;

typedef enum TspParserStatus {
    PARSE_OK = 0,
    PARSE_ERR_OPEN = -1,
    PARSE_ERR_FORMAT = -2,
    PARSE_ERR_MEMORY = -3,
    PARSE_ERR_EXT = -4,
    PARSE_ERR_DIM = -5,
    PARSE_ERR_INV = -6,
    PARSE_ERR_OVERFLOW = -7
} TspParserStatus;


TspParserStatus tsp_parser_load_instance(const char *path, Node **out_nodes, int *out_n);

TspParserStatus tsp_parser_load_solution(const char *path, int n, int *tour_buf, double *out_cost);

TspError tsp_error_from_parser_status(TspParserStatus st);

#endif
