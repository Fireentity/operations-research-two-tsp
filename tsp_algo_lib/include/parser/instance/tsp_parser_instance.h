#ifndef TSP_PARSER_INSTANCE_H
#define TSP_PARSER_INSTANCE_H

#include "tsp_instance.h"

TspParserStatus parse_instance_tsplib(const char *path, Node **out_nodes, int *out_n);

#endif
