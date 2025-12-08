#include "parser_test.h"
#include <stdio.h>
#include <assert.h>
#include <math.h>

#include "constants.h"
#include "tsp_instance.h"
#include "tsp_solution.h"

static void test_instance_parsing(void) {
    printf("\t[Parser] Testing Instance Load...\n");
    const char *filename = "test_dummy.tsp";
    FILE *f = fopen(filename, "w");
    assert(f != NULL);
    fprintf(f, "NAME: test\n");
    fprintf(f, "TYPE: TSP\n");
    fprintf(f, "DIMENSION: 3\n");
    fprintf(f, "EDGE_WEIGHT_TYPE: EUC_2D\n");
    fprintf(f, "NODE_COORD_SECTION\n");
    fprintf(f, "1 0.0 0.0\n");
    fprintf(f, "2 3.0 4.0\n");
    fprintf(f, "3 6.0 0.0\n");
    fprintf(f, "EOF\n");
    fclose(f);

    TspInstance *inst = NULL;
    TspError err = tsp_instance_load_from_file(&inst, filename);

    assert(err == TSP_OK);
    assert(inst != NULL);
    assert(tsp_instance_get_num_nodes(inst) == 3);

    const Node *nodes = tsp_instance_get_nodes(inst);
    assert(nodes[0].x == 0.0 && nodes[0].y == 0.0);
    assert(nodes[1].x == 3.0 && nodes[1].y == 4.0);

    const double *costs = tsp_instance_get_cost_matrix(inst);
    double d12 = costs[0 * 3 + 1];
    assert(fabs(d12 - 5.0) < EPSILON);

    tsp_instance_destroy(inst);
    remove(filename);
}

static void test_solution_save_load(void) {
    printf("\t[Parser] Testing Solution Save/Load...\n");
    const char *filename = "test_sol_rw.sol";

    Node nodes[] = {{0,0}, {10,0}, {10,10}, {0,10}};
    TspInstance *inst = tsp_instance_create(nodes, 4);

    int custom_tour[] = {0, 2, 1, 3, 0};
    TspSolution *sol_orig = tsp_solution_create_with_tour(inst, custom_tour);
    double orig_cost = tsp_solution_get_cost(sol_orig);

    int res = tsp_solution_save(sol_orig, filename);
    assert(res == TSP_OK);

    TspSolution *sol_loaded = tsp_solution_create(inst);
    res = tsp_solution_load(sol_loaded, filename);
    assert(res == TSP_OK);

    double loaded_cost = tsp_solution_get_cost(sol_loaded);
    assert(fabs(orig_cost - loaded_cost) < EPSILON);

    int tour_buf[5];
    tsp_solution_get_tour(sol_loaded, tour_buf);

    for(int i = 0; i < 5; i++) {
        assert(tour_buf[i] == custom_tour[i]);
    }

    tsp_solution_destroy(sol_orig);
    tsp_solution_destroy(sol_loaded);
    tsp_instance_destroy(inst);
    remove(filename);
}

static void test_parser_failures(void) {
    printf("\t[Parser] Testing Error Handling...\n");
    TspInstance *inst = NULL;

    TspError err = tsp_instance_load_from_file(&inst, "non_existent_ghost_file.tsp");
    assert(err == TSP_ERR_FILE_OPEN);
    assert(inst == NULL);

    const char *bad_dim_file = "test_bad_dim.tsp";
    FILE *f = fopen(bad_dim_file, "w");
    fprintf(f, "NAME: bad_dim\nTYPE: TSP\nDIMENSION: 10\nNODE_COORD_SECTION\n");
    fprintf(f, "1 0 0\n2 0 0\nEOF\n");
    fclose(f);

    err = tsp_instance_load_from_file(&inst, bad_dim_file);
    if (inst) tsp_instance_destroy(inst);
    remove(bad_dim_file);

    const char *bad_data_file = "test_bad_data.tsp";
    f = fopen(bad_data_file, "w");
    fprintf(f, "DIMENSION: 3\nNODE_COORD_SECTION\n");
    fprintf(f, "1 0 0\n2 GARBAGE 0\n3 0 0\nEOF\n");
    fclose(f);

    err = tsp_instance_load_from_file(&inst, bad_data_file);
    assert(err != TSP_OK);

    if (inst) tsp_instance_destroy(inst);
    remove(bad_data_file);
}

static void test_solution_validation_errors(void) {
    printf("\t[Parser] Testing Solution Validation Errors...\n");
    const char *sol_file = "test_bad_sol.sol";

    Node nodes[] = {{0,0}, {1,1}, {2,2}};
    TspInstance *inst = tsp_instance_create(nodes, 3);
    TspSolution *sol = tsp_solution_create(inst);

    FILE *f = fopen(sol_file, "w");
    fprintf(f, "TSP_SOLUTION_V1\nCOST 0.0\nDIMENSION 3\nTOUR_SECTION\n");
    fprintf(f, "0 1 1 0\nEOF\n");
    fclose(f);

    TspError res = tsp_solution_load(sol, sol_file);
    assert(res == TSP_ERR_PARSE_NODES);

    f = fopen(sol_file, "w");
    fprintf(f, "TSP_SOLUTION_V1\nCOST 0.0\nDIMENSION 3\nTOUR_SECTION\n");
    fprintf(f, "0 1 99 0\nEOF\n");
    fclose(f);

    res = tsp_solution_load(sol, sol_file);
    assert(res == TSP_ERR_PARSE_NODES);

    tsp_solution_destroy(sol);
    tsp_instance_destroy(inst);
    remove(sol_file);
}

void run_parser_tests(void) {
    printf("[Parser] Running tests...\n");
    test_instance_parsing();
    test_solution_save_load();
    test_parser_failures();
    test_solution_validation_errors();
    printf("[Parser] Passed.\n");
}