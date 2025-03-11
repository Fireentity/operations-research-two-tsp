#ifndef TSP_INSTANCE_H
#define TSP_INSTANCE_H
typedef struct
{
    int x_square;
    int y_square;
    unsigned int square_side;
} Rectangle;

typedef struct
{
    double x;
    double y;
} Node;

typedef struct TspInstance TspInstance;

const TspInstance* init_random_tsp_instance(unsigned long number_of_nodes,int seed, Rectangle generation_area);
const double* get_edge_cost_array(const TspInstance* instance);
unsigned long get_number_of_nodes(const TspInstance* instance);
const Node* get_nodes(const TspInstance* instance);

#endif //TSP_INSTANCE_H
