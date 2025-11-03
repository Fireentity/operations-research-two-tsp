#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "algorithms.h"

// Helper function to create a simple edge cost matrix for testing
void create_simple_edge_costs(double* edge_costs, const int num_nodes) {
    // Creating a simple fully connected matrix with random costs for simplicity
    for (int i = 0; i < num_nodes; i++) {
        for (int j = 0; j < num_nodes; j++) {
            if (i != j) {
                edge_costs[i * num_nodes + j] = (double)(rand() % 100 + 1); // Random costs between 1 and 100
            }
        }
    }
}

// Test for a basic case with 3 nodes
void test_grasp_nearest_neighbor_tour_basic_case() {
    const int num_nodes = 5;
    int tour[num_nodes + 1]; // Include the last node to close the tour
    double cost = 0.0;
    double edge_costs[num_nodes * num_nodes];
    create_simple_edge_costs(edge_costs, num_nodes);

    assert(0 == grasp_nearest_neighbor_tour(0, tour, num_nodes, edge_costs, &cost, 0.5, 0.3, 0.2));

    // Verify the tour is completed and cost is updated
    assert(tour[0] == tour[num_nodes]); // Ensure it's a closed tour
    assert(cost > 0); // Ensure a valid cost is calculated
}

// Test for boundary conditions, starting node out of range
void test_grasp_nearest_neighbor_tour_invalid_starting_node() {
    const int num_nodes = 3;
    int tour[num_nodes + 1];
    double cost = 0.0;
    double edge_costs[num_nodes * num_nodes];
    create_simple_edge_costs(edge_costs, num_nodes);

    // Test for out-of-bounds starting node
    assert(-1 == grasp_nearest_neighbor_tour(4, tour, num_nodes, edge_costs, &cost, 0.5, 0.3, 0.2));
}

// More tests can be added here...

void run_grasp_nn_helpers_tests(void) {
    printf("--- Running GRASP-NN Helper Tests ---\n");
    test_grasp_nearest_neighbor_tour_basic_case();
    test_grasp_nearest_neighbor_tour_invalid_starting_node();
    printf("GRASP-NN Helper tests passed.\n");
}
