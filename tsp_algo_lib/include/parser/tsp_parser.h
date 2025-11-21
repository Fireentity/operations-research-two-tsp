#ifndef TSP_PARSER_H
#define TSP_PARSER_H

#include "tsp_instance.h"

typedef TspError (*TspParserFn)(const char *path, Node **out_nodes, int *out_n);

/**
 * @brief Dispatches parsing using registered parsers based on file extension.
 */
TspError tsp_parse_by_extension(const char *path, Node **out_nodes, int *out_n);

#endif
