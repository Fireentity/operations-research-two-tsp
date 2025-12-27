#include "subtour_separator.h"
#include "cplex_solver_wrapper.h" // For xpos
#include "c_util.h"
#include <stdlib.h>
#include <string.h>

// Helper structure for sparse graph traversal
typedef struct {
    int *head;
    int *next;
    int *to;
    int edge_count;
} AdjacencyList;

ConnectedComponents *connected_components_create(int num_nodes) {
    ConnectedComponents *cc = malloc(sizeof(ConnectedComponents));
    check_alloc(cc);
    cc->component_of_node = malloc(num_nodes * sizeof(int));
    cc->nodes_in_component = malloc(num_nodes * sizeof(int)); // Helper counter
    check_alloc(cc->component_of_node);
    check_alloc(cc->nodes_in_component);
    return cc;
}

void connected_components_destroy(ConnectedComponents *cc) {
    if (!cc) return;
    free(cc->component_of_node);
    free(cc->nodes_in_component);
    free(cc);
}

// Iterative DFS to mark connected components
static void dfs(int start_node, int comp_id, int *comp_array, const AdjacencyList *adj) {
    // Stack size is conservative (num edges)
    int *stack = malloc((adj->edge_count + 100) * sizeof(int));
    check_alloc(stack);
    int top = 0;

    stack[top++] = start_node;
    comp_array[start_node] = comp_id;

    while (top > 0) {
        int u = stack[--top];

        // Iterate neighbors of u
        for (int e = adj->head[u]; e != -1; e = adj->next[e]) {
            int v = adj->to[e];
            if (comp_array[v] == -1) {
                comp_array[v] = comp_id;
                stack[top++] = v;
            }
        }
    }
    free(stack);
}

void find_connected_components(ConnectedComponents *cc, int num_nodes, const double *x_star) {
    // 1. Reset state
    cc->num_components = 0;
    for (int i = 0; i < num_nodes; i++) {
        cc->component_of_node[i] = -1;
        cc->nodes_in_component[i] = 0; // Not strictly used for logic but good for debugging
    }

    // 2. Build temporary Adjacency List from x_star solution
    // We assume x_star contains edges (val > 0.5)
    // Max directed edges for a tour is 2*N
    int max_edges = num_nodes * 4;
    int *adj_head = malloc(num_nodes * sizeof(int));
    int *adj_next = malloc(max_edges * sizeof(int));
    int *adj_to = malloc(max_edges * sizeof(int));
    check_alloc(adj_head);
    check_alloc(adj_next);
    check_alloc(adj_to);

    // Initialize list heads to -1 (end of list)
    memset(adj_head, -1, num_nodes * sizeof(int));

    int edge_idx = 0;

    for (int i = 0; i < num_nodes; i++) {
        for (int j = i + 1; j < num_nodes; j++) {
            int idx = xpos(i, j, num_nodes);

            // If edge is selected in CPLEX solution
            if (x_star[idx] > 0.5) {
                // Add Edge i -> j
                adj_to[edge_idx] = j;
                adj_next[edge_idx] = adj_head[i];
                adj_head[i] = edge_idx++;

                // Add Edge j -> i
                adj_to[edge_idx] = i;
                adj_next[edge_idx] = adj_head[j];
                adj_head[j] = edge_idx++;
            }
        }
    }

    AdjacencyList adj = {.head = adj_head, .next = adj_next, .to = adj_to, .edge_count = edge_idx};

    // 3. Run DFS on unvisited nodes to find components
    for (int i = 0; i < num_nodes; i++) {
        if (cc->component_of_node[i] == -1) {
            cc->num_components++;
            dfs(i, cc->num_components, cc->component_of_node, &adj);
        }
    }

    // 4. Cleanup
    free(adj_head);
    free(adj_next);
    free(adj_to);
}
