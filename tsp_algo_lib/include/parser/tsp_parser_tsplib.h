#ifndef TSP_PARSER_TSPLIB_H
#define TSP_PARSER_TSPLIB_H

#include "tsp_instance.h"

/**
 * @brief Parses a TSPLIB (.tsp) file into nodes.
 */
TspError tsp_parse_tsplib(const char *path, Node **out_nodes, int *out_n);

#endif