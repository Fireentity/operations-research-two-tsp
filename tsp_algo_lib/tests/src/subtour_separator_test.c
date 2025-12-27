#include "subtour_separator_test.h"
#include "subtour_separator.h"
#include "cplex_solver_wrapper.h" // Per xpos
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

void test_two_disjoint_triangles() {
    printf("  [Subtour] Testing 2 disjoint triangles (6 nodes)...\n");
    int n = 6;
    // Grafo: 0-1-2-0 (componente 1) e 3-4-5-3 (componente 2)
    // Numero di variabili CPLEX (n*(n-1)/2) = 15
    int num_vars = (n * (n - 1)) / 2;
    double *x_star = calloc(num_vars, sizeof(double));

    // Impostiamo a 1.0 gli archi dei due triangoli
    // Triangolo 1: (0,1), (1,2), (0,2)
    x_star[xpos(0, 1, n)] = 1.0;
    x_star[xpos(1, 2, n)] = 1.0;
    x_star[xpos(0, 2, n)] = 1.0;

    // Triangolo 2: (3,4), (4,5), (3,5)
    x_star[xpos(3, 4, n)] = 1.0;
    x_star[xpos(4, 5, n)] = 1.0;
    x_star[xpos(3, 5, n)] = 1.0;

    ConnectedComponents *cc = connected_components_create(n);
    find_connected_components(cc, n, x_star);

    // Verifica
    assert(cc->num_components == 2);

    // Verifica appartenenza nodi (ID componenti partono da 1)
    int comp_id_0 = cc->component_of_node[0];
    assert(cc->component_of_node[1] == comp_id_0);
    assert(cc->component_of_node[2] == comp_id_0);

    int comp_id_3 = cc->component_of_node[3];
    assert(cc->component_of_node[4] == comp_id_3);
    assert(cc->component_of_node[5] == comp_id_3);

    assert(comp_id_0 != comp_id_3);

    connected_components_destroy(cc);
    free(x_star);
    printf("  [Subtour] Passed.\n");
}

void test_single_tour() {
    printf("  [Subtour] Testing single tour (4 nodes)...\n");
    int n = 4;
    int num_vars = (n * (n - 1)) / 2;
    double *x_star = calloc(num_vars, sizeof(double));

    // Tour: 0-1-2-3-0
    x_star[xpos(0, 1, n)] = 1.0;
    x_star[xpos(1, 2, n)] = 1.0;
    x_star[xpos(2, 3, n)] = 1.0;
    x_star[xpos(0, 3, n)] = 1.0;

    ConnectedComponents *cc = connected_components_create(n);
    find_connected_components(cc, n, x_star);

    assert(cc->num_components == 1);
    for(int i=0; i<n; i++) {
        assert(cc->component_of_node[i] == 1); // Solitamente ID 1
    }

    connected_components_destroy(cc);
    free(x_star);
    printf("  [Subtour] Passed.\n");
}

void run_subtour_separator_tests(void) {
    printf("[Subtour Separator] Running tests...\n");
    test_two_disjoint_triangles();
    test_single_tour();
    printf("[Subtour Separator] All tests passed.\n");
}