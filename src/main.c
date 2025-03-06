#include <stdio.h>
#include <stdlib.h>
#include "tsp_utilities.h"
#include "parsing_util.h"

// Function to parse the command-line arguments using flags.
void parse_command_line(const int argc, char **argv, TspParams *params) {

    if (argc < 2) {
        fprintf(stderr,
            "Usage: %s [OPTION]...\n"
            "Generates and solves Traveling Salesman Problem (TSP) instances with specified parameters.\n"
            "\n"
            "Examples:\n"
            "  tsp_solver --nodes 10 --seed 42 --x-square 5 --y-square 5 --square-side 10\n"
            "  tsp_solver --nodes 20 --seed 99 --x-square 3 --y-square 3 --square-side 15\n"
            "\n"
            "Mandatory options:\n"
            "      --nodes=N               specify number of nodes (N must be > 0)\n"
            "      --seed=N                set random seed for reproducibility\n"
            "      --x-square=N            define x-coordinate of the square\n"
            "      --y-square=N            define y-coordinate of the square\n"
            "      --square-side=N         set side length of the square\n"
            "\n"
            "Other options:\n"
            "  -?, --help                 display this help message\n"
            "      --version              output version information\n",
            argv[0]);

        fprintf(stderr, "Example: %s -nodes 100 -seed 42 -x_min 0 -y_min 0 -x_max 100 -y_max 100\n\n", argv[0]);
        exit(1);
    }

    for (int i = 1; i < argc-1; i++) {
        if (argc-i>0)
        {
            parse_flag(argv[i], argv[i+1], params);
            i++;
        }
    }
}

int main(const int argc, char *argv[]) {
    TspParams params;
    parse_command_line(argc, argv, &params);

    //const double start_seconds = second();

    printf("%ld", params.number_of_nodes);

    // Generate random points using the parsed parameters.
    const Node *nodes = generate_random_points(params.rectangle, params.number_of_nodes, params.seed);

    for (int i = 0; i < params.number_of_nodes; i++) {
        printf("Node[%d]\t(%ld,%ld)\n", i, nodes[i].x, nodes[i].y);
    }

    //const double end_seconds = second();
    //printf("Nodes generated in %lf sec.s\n", end_seconds - start_seconds);

    return 0;
}
