#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "tsp_utilities.h"
#include "chrono.c"
#include "parsing/parsing_util.h"

// Function to parse the command-line arguments using flags.
void parse_command_line(const int argc, char **argv, TspParams *params) {

    if (argc < 2) {
        fprintf(
            stderr,
            "\nUsage: %s -nodes <number_of_nodes> -seed <seed> -x_min <x_min> -y_min <y_min> -x_max <x_max> -y_max <y_max>\n",
            argv[0]);
        fprintf(stderr, "Example: %s -nodes 100 -seed 42 -x_min 0 -y_min 0 -x_max 100 -y_max 100\n\n", argv[0]);
        exit(1);
    }

    for (int i = 1; i < argc; i++) {
        parse_flag(argv[i], params);
    }
}

int main(const int argc, char *argv[]) {
    TspParams params;
    parse_command_line(argc, argv, &params);

    const double start_seconds = second();

    // Generate random points using the parsed parameters.
    const Node *nodes = generate_random_points(params.rectangle, params.number_of_nodes, params.seed);

    for (int i = 0; i < params.number_of_nodes; i++) {
        printf("Node[%d]\t(%ld,%ld)\n", i, nodes[i].x, nodes[i].y);
    }

    const double end_seconds = second();
    printf("Nodes generated in %lf sec.s\n", end_seconds - start_seconds);

    return 0;
}
