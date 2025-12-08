#ifndef TSP_PARSER_SOLUTION_H
#define TSP_PARSER_SOLUTION_H
#include "tsp_parser.h"

#define SOL_V1_MAGIC        "TSP_SOLUTION_V1"
#define SOL_V1_COST         "COST"
#define SOL_V1_DIM          "DIMENSION"
#define SOL_V1_TOUR_SECTION "TOUR_SECTION"
#define SOL_V1_EOF          "EOF"

TspParserStatus parse_solution_v1(const char *path, int n, int *tour_buf, double *out_cost);

#endif
