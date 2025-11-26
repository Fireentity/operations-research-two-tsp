#include "parser_test.h"
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include "tsp_instance.h"

void run_parser_tests(void) {
    printf("[Parser] Running tests...\n");

    // 1. Create a dummy .tsp file
    const char *filename = "test_dummy.tsp";
    FILE *f = fopen(filename, "w");
    assert(f != NULL);
    fprintf(f, "NAME: test\n");
    fprintf(f, "TYPE: TSP\n");
    fprintf(f, "DIMENSION: 3\n");
    fprintf(f, "EDGE_WEIGHT_TYPE: EUC_2D\n");
    fprintf(f, "NODE_COORD_SECTION\n");
    fprintf(f, "1 0.0 0.0\n");
    fprintf(f, "2 3.0 4.0\n"); // Distance from 1 is 5.0
    fprintf(f, "3 6.0 0.0\n");
    fprintf(f, "EOF\n");
    fclose(f);

    // 2. Load it using your library
    TspInstance *inst = NULL;
    TspError err = tsp_instance_load_from_file(&inst, filename);

    // 3. Validate Loading
    assert(err == TSP_OK);
    assert(inst != NULL);
    assert(tsp_instance_get_num_nodes(inst) == 3);

    // 4. Validate Coordinates
    const Node *nodes = tsp_instance_get_nodes(inst);
    assert(nodes[0].x == 0.0 && nodes[0].y == 0.0);
    assert(nodes[1].x == 3.0 && nodes[1].y == 4.0);

    // 5. Validate Cost Matrix Calculation
    const double *costs = tsp_instance_get_cost_matrix(inst);
    // Dist 1-2 (Index 0->1) should be sqrt(3^2 + 4^2) = 5
    double d12 = costs[0 * 3 + 1];
    assert(fabs(d12 - 5.0) < 1e-6);

    // 6. Cleanup
    tsp_instance_destroy(inst);
    remove(filename);

    printf("[Parser] Passed.\n");
}