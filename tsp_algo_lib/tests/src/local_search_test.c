#include "local_search_test.h"
#include <stdio.h>
#include <assert.h>
#include "tsp_instance.h"
#include "local_search.h"
#include "time_limiter.h"

void run_local_search_tests(void) {
    printf("[Local Search] Running 2-Opt Logic tests...\n");

    // Create a "Crossed Square" (Bowtie shape)
    // 0(0,0)       1(1,0)
    //      \     /
    //       \   /
    //        \ /
    //         X
    //        / \
    //       /   \
    //      /     \
    // 3(0,1)       2(1,1)

    // Standard Perimeter: 0->1 (1.0), 1->2 (1.0), 2->3 (1.0), 3->0 (1.0) = 4.0
    // Crossed Tour: 0 -> 2 -> 1 -> 3 -> 0
    // Edges: (0,0)->(1,1) [sqrt 2] + (1,1)->(1,0) [1.0] + (1,0)->(0,1) [sqrt 2] + (0,1)->(0,0) [1.0]
    // Crossed Cost: 2 + 2*sqrt(2) approx 4.828

    Node nodes[] = {{0, 0}, {1, 0}, {1, 1}, {0, 1}};
    TspInstance *inst = tsp_instance_create(nodes, 4);
    const double *costs = tsp_instance_get_cost_matrix(inst);

    // Manually set a crossed tour: 0 -> 2 -> 1 -> 3 -> 0
    int tour[] = {0, 2, 1, 3, 0};

    TimeLimiter timer = time_limiter_create(1.0); // 1 sec limit
    time_limiter_start(&timer);

    // Run 2-Opt
    double improvement = two_opt(tour, 4, costs, timer);

    // Validation
    // 1. Improvement should be negative (cost reduction)
    assert(improvement < -0.1);

    // 2. Check edges. Valid perimeter tour is 0-1-2-3 or 0-3-2-1
    // Let's check if node 0 is connected to 1 or 3 (not 2)
    int next_to_0 = tour[1];
    assert(next_to_0 == 1 || next_to_0 == 3);

    tsp_instance_destroy(inst);
    printf("[Local Search] Passed.\n");
}
