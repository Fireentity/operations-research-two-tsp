#include "test_instances.h"
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
    fprintf(f, "NAME: test\nTYPE: TSP\nDIMENSION: 3\nEDGE_WEIGHT_TYPE: EUC_2D\nNODE_COORD_SECTION\n");
    fprintf(f, "1 0.0 0.0\n2 3.0 4.0\n3 6.0 0.0\nEOF\n");
    fclose(f);

    TspInstance *inst = NULL;
    TspError err = tsp_instance_load_from_file(&inst, filename);

    assert(err == TSP_OK);
    assert(inst != NULL);
    assert(tsp_instance_get_num_nodes(inst) == 3);

    const double *costs = tsp_instance_get_cost_matrix(inst);
    double d12 = costs[0 * 3 + 1];
    assert(fabs(d12 - 5.0) < EPSILON_EXACT);

    tsp_instance_destroy(inst);
    remove(filename);
}

static void test_solution_save_load(void) {
    printf("\t[Parser] Testing Solution Save/Load...\n");
    const char *filename = "test_sol_rw.sol";

    TspInstance *inst = create_square_instance();
    int custom_tour[] = {0, 2, 1, 3, 0};
    TspSolution *sol_orig = tsp_solution_create_with_tour(inst, custom_tour);

    int res = tsp_solution_save(sol_orig, filename);
    assert(res == TSP_OK);

    TspSolution *sol_loaded = tsp_solution_create(inst);
    res = tsp_solution_load(sol_loaded, filename);
    assert(res == TSP_OK);

    double loaded_cost = tsp_solution_get_cost(sol_loaded);
    assert(fabs(tsp_solution_get_cost(sol_orig) - loaded_cost) < EPSILON_EXACT);

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
}

void run_parser_tests(void) {
    printf("[Parser] Running tests...\n");
    test_instance_parsing();
    test_solution_save_load();
    test_parser_failures();
    printf("[Parser] Passed.\n");
}