#include "test_instances.h"
#include <stdio.h>
#include <assert.h>
#include "local_search.h"
#include "time_limiter.h"
#include "tsp_solution.h"
#include "tsp_math.h"

static void test_two_opt_crossed_square(void) {
    printf("  [Local Search] Testing 2-Opt Crossed Square...\n");

    TspInstance *inst = create_square_instance();
    const double *costs = tsp_instance_get_cost_matrix(inst);
    int n = 4;

    /*
       0 -> 2 -> 1 -> 3 -> 0 (Crossed)
       Optimal: 0 -> 1 -> 2 -> 3 -> 0 (Perimeter)
    */
    int tour[] = {0, 2, 1, 3, 0};

    TimeLimiter timer = time_limiter_create(1.0);
    time_limiter_start(&timer);

    double improvement = two_opt(tour, n, costs, timer);

    printf("    Improvement: %.4f\n", improvement);
    assert(improvement < -8.0); // Approx -8.28

    // Verify connectivity
    int next = tour[1];
    int prev = tour[3];
    assert(next == 1 || next == 3);
    assert(prev == 1 || prev == 3);

    tsp_instance_destroy(inst);
}

static void test_two_opt_random_improvement(void) {
    printf("  [Local Search] Testing 2-Opt on Random Instance...\n");

    TspInstance *inst = create_random_instance_100();
    const double *costs = tsp_instance_get_cost_matrix(inst);
    int n = 100;

    // Create trivial identity tour 0-1-2...-99-0
    int *tour = malloc((n + 1) * sizeof(int));
    for(int i=0; i<n; i++) tour[i] = i;
    tour[n] = 0;

    double initial_cost = calculate_tour_cost(tour, n, costs);

    TimeLimiter timer = time_limiter_create(1.0);
    time_limiter_start(&timer);

    double improvement = two_opt(tour, n, costs, timer);
    double final_cost = calculate_tour_cost(tour, n, costs);

    // 2-Opt is descent local search, cost must not increase
    assert(final_cost <= initial_cost + EPSILON_EXACT);
    assert(fabs((final_cost - initial_cost) - improvement) < EPSILON_EXACT);

    free(tour);
    tsp_instance_destroy(inst);
}

void run_local_search_tests(void) {
    printf("[Local Search] Running tests...\n");
    test_two_opt_crossed_square();
    test_two_opt_random_improvement();
    printf("[Local Search] All tests passed.\n");
}