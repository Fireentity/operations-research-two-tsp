#include <stdio.h>
#include <stdlib.h>
#include "tsp_utilities.h"

Node* generate_random_points(const Rectangle rect, const long number_of_nodes, const long seed) {
    srand(seed);
    Node* points = malloc(number_of_nodes * sizeof(Node));
    if (!points) {
        fprintf(stderr, "Allocation error\n");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < number_of_nodes; i++) {
        points[i].x = rect.x_square + rand() % (rect.square_side + 1);
        points[i].y = rect.y_square + rand() % (rect.square_side + 1);
    }
    return points;
}
