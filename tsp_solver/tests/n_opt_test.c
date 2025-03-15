#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define SWAP(a, b) do { __typeof__(a) _tmp = (a); (a) = (b); (b) = _tmp; } while (0)
#define REVERSE_ARRAY(arr, start, end)                   \
    for (size_t i = (start), j = (end); i < j; i++, j--) { \
        SWAP((arr)[i], (arr)[j]);                        \
    }

static void rotate_left(int* arr, int len, int shift) {
    if (shift <= 0 || shift >= len) return;
    REVERSE_ARRAY(arr, 0, len - 1);
    REVERSE_ARRAY(arr, 0, len - shift - 1);
    REVERSE_ARRAY(arr, len - shift, len - 1);
}

void n_opt_move(int* tour, int number_of_nodes, const double* edge_cost_array,
                double* cost, int n, const int* edges) {
    if (n < 2) return;
    for (int k = 0; k < n - 1; k++) {
        int start = edges[k] + 1;
        int end = edges[k + 1];
        if (start <= end) {
            REVERSE_ARRAY(tour, start, end);
        } else {
            REVERSE_ARRAY(tour, start, number_of_nodes - 1);
            REVERSE_ARRAY(tour, 0, end);
            rotate_left(tour, number_of_nodes, number_of_nodes - start);
        }
    }
    double new_cost = 0.0;
    for (int i = 0; i < number_of_nodes - 1; i++)
        new_cost += edge_cost_array[tour[i] * number_of_nodes + tour[i + 1]];
    new_cost += edge_cost_array[tour[number_of_nodes - 1] * number_of_nodes + tour[0]];
    *cost = new_cost;
}

double* create_cost_matrix(int number_of_nodes) {
    double* matrix = malloc(sizeof(double) * number_of_nodes * number_of_nodes);
    if (!matrix) exit(EXIT_FAILURE);
    for (int i = 0; i < number_of_nodes; i++)
        for (int j = 0; j < number_of_nodes; j++)
            matrix[i * number_of_nodes + j] = abs(i - j);
    return matrix;
}

double compute_cost(const int* tour, int number_of_nodes, const double* edge_cost_array) {
    double total = 0.0;
    for (int i = 0; i < number_of_nodes - 1; i++)
        total += edge_cost_array[tour[i] * number_of_nodes + tour[i + 1]];
    total += edge_cost_array[tour[number_of_nodes - 1] * number_of_nodes + tour[0]];
    return total;
}

void test_case1_all_right() {
    // n < 2: no change
    int tour[4] = {0, 1, 2, 3};
    int expected[4] = {0, 1, 2, 3};
    int number_of_nodes = 4;
    double* cost_matrix = create_cost_matrix(number_of_nodes);
    double cost;
    int n = 1;
    int edges[1] = {0};

    n_opt_move(tour, number_of_nodes, cost_matrix, &cost, n, edges);
    for (int i = 0; i < number_of_nodes; i++)
        assert(tour[i] == expected[i]);
    assert(cost == compute_cost(expected, number_of_nodes, cost_matrix));
    free(cost_matrix);
}

void test_case2() {
    // n = 2: reverse segment from index 2 to 3
    int tour[4] = {0, 1, 2, 3};
    int expected[4] = {0, 1, 3, 2};
    int number_of_nodes = 4;
    double* cost_matrix = create_cost_matrix(number_of_nodes);
    double cost;
    int n = 2;
    int edges[2] = {1, 3};

    n_opt_move(tour, number_of_nodes, cost_matrix, &cost, n, edges);
    for (int i = 0; i < number_of_nodes; i++)
        assert(tour[i] == expected[i]);
    assert(cost == compute_cost(expected, number_of_nodes, cost_matrix));
    free(cost_matrix);
}

void test_case3() {
    // n = 3: wrap-around reversal
    int tour[6] = {0, 1, 2, 3, 4, 5};
    int expected[6] = {0, 2, 3, 4, 5, 1};
    int number_of_nodes = 6;
    double* cost_matrix = create_cost_matrix(number_of_nodes);
    double cost;
    int n = 3;
    int edges[3] = {4, 1, 3};

    n_opt_move(tour, number_of_nodes, cost_matrix, &cost, n, edges);
    for (int i = 0; i < number_of_nodes; i++)
        assert(tour[i] == expected[i]);
    assert(cost == compute_cost(expected, number_of_nodes, cost_matrix));
    free(cost_matrix);
}

void test_case4() {
    // Single node
    int tour[1] = {0};
    int expected[1] = {0};
    int number_of_nodes = 1;
    double* cost_matrix = create_cost_matrix(number_of_nodes);
    double cost;
    int n = 2;
    int edges[2] = {0, 0};

    n_opt_move(tour, number_of_nodes, cost_matrix, &cost, n, edges);
    assert(tour[0] == expected[0]);
    assert(cost == compute_cost(expected, number_of_nodes, cost_matrix));
    free(cost_matrix);
}

int main() {
    test_case1_all_right();
    test_case2();
    test_case3();
    test_case4();
    printf("All tests passed.\n");
    return 0;
}
