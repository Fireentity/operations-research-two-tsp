#ifndef FUNCTION_TYPE_H
#define FUNCTION_TYPE_H
typedef void (*TspSolver)(
        unsigned long *tour,
        unsigned long starting_node,
        unsigned long number_of_nodes,
        const double *edge_cost_array,
        double *cost
);
#endif //FUNCTION_TYPE_H
