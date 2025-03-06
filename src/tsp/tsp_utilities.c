#include <stdio.h>
#include <stdlib.h>
#include "tsp_utilities.h"
#include "parsing_util.h"

Node* generate_random_nodes(const Rectangle rect, const long number_of_nodes, const long seed) {
    srand(seed);
    Node* nodes = malloc(number_of_nodes * sizeof(Node));
    if (!nodes) {
        fprintf(stderr, "Allocation error\n");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < number_of_nodes; i++) {
        nodes[i].x = rect.x_square + rand() % (rect.square_side + 1);
        nodes[i].y = rect.y_square + rand() % (rect.square_side + 1);
    }
    return nodes;
}
