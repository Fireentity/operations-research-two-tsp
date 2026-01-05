#include "test_instances.h"
#include "subtour_separator.h"
#include "cplex_solver_wrapper.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "c_util.h"

static void test_two_disjoint_triangles(void) {
    printf("  [Subtour] Testing 2 disjoint triangles (6 nodes)...\n");
    int n = 6;
    int num_vars = (n * (n - 1)) / 2;
    double *x_star = tsp_calloc(num_vars, sizeof(double));

    // Component 1: 0-1-2-0
    x_star[xpos(0, 1, n)] = 1.0;
    x_star[xpos(1, 2, n)] = 1.0;
    x_star[xpos(0, 2, n)] = 1.0;

    // Component 2: 3-4-5-3
    x_star[xpos(3, 4, n)] = 1.0;
    x_star[xpos(4, 5, n)] = 1.0;
    x_star[xpos(3, 5, n)] = 1.0;

    ConnectedComponents *cc = connected_components_create(n);
    find_connected_components(cc, n, x_star);

    assert(cc->num_components == 2);

    int comp_id_0 = cc->component_of_node[0];
    assert(cc->component_of_node[1] == comp_id_0);
    assert(cc->component_of_node[2] == comp_id_0);

    int comp_id_3 = cc->component_of_node[3];
    assert(cc->component_of_node[4] == comp_id_3);
    assert(cc->component_of_node[5] == comp_id_3);

    assert(comp_id_0 != comp_id_3);

    connected_components_destroy(cc);
    tsp_free(x_star);
}

static void test_single_tour(void) {
    printf("  [Subtour] Testing single tour (4 nodes)...\n");
    int n = 4;
    int num_vars = (n * (n - 1)) / 2;
    double *x_star = tsp_calloc(num_vars, sizeof(double));

    // Tour: 0-1-2-3-0
    x_star[xpos(0, 1, n)] = 1.0;
    x_star[xpos(1, 2, n)] = 1.0;
    x_star[xpos(2, 3, n)] = 1.0;
    x_star[xpos(0, 3, n)] = 1.0;

    ConnectedComponents *cc = connected_components_create(n);
    find_connected_components(cc, n, x_star);

    assert(cc->num_components == 1);
    for (int i = 0; i < n; i++) {
        assert(cc->component_of_node[i] == 1);
    }

    connected_components_destroy(cc);
    tsp_free(x_star);
}

void run_subtour_separator_tests(void) {
    printf("[Subtour Separator] Running tests...\n");
    test_two_disjoint_triangles();
    test_single_tour();
    printf("[Subtour Separator] All tests passed.\n");
}