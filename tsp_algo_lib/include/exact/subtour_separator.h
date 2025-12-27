#ifndef SUBTOUR_SEPARATOR_H
#define SUBTOUR_SEPARATOR_H

typedef struct {
    int num_components;
    int *component_of_node;
    int *nodes_in_component;
} ConnectedComponents;

/**
 * Allocates the structure to hold component data.
 */
ConnectedComponents *connected_components_create(int num_nodes);

/**
 * Frees the structure.
 */
void connected_components_destroy(ConnectedComponents *cc);

/**
 * Identifies connected components in the graph defined by x_star.
 * * @param cc Structure to populate.
 * @param num_nodes Total nodes.
 * @param x_star The binary solution from CPLEX (size n*(n-1)/2).
 * Values > 0.5 are considered edges.
 */
void find_connected_components(ConnectedComponents *cc, int num_nodes, const double *x_star);

#endif // SUBTOUR_SEPARATOR_H
