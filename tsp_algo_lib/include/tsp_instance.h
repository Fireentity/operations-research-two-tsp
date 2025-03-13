#ifndef TSP_INSTANCE_H
#define TSP_INSTANCE_H

typedef struct
{
    const int x_square;
    const int y_square;
    const unsigned int square_side;
} TspGenerationArea;

typedef struct
{
    double x;
    double y;
} Node;

typedef struct TspInstanceState TspInstanceState;

typedef struct TspInstance TspInstance;

struct TspInstance
{
    TspInstanceState* state;
    const double* (*get_edge_cost_array)(const TspInstance* self);
    int (*get_number_of_nodes)(const TspInstance* self);
    const Node* (*get_nodes)(const TspInstance* self);
};

const TspInstance* init_random_tsp_instance(int number_of_nodes,
                                            int seed,
                                            TspGenerationArea generation_area);

const TspInstance* init_tsp_instance(const Node* nodes, int number_of_nodes);

#endif //TSP_INSTANCE_H
