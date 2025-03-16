#ifndef TSP_INSTANCE_H
#define TSP_INSTANCE_H

/**
 * @brief Defines the area used for generating a random TSP instance.
 */
typedef struct
{
    const int x_square;         /**< X-coordinate of the square grid. */
    const int y_square;         /**< Y-coordinate of the square grid. */
    const unsigned int square_side; /**< Side length of the square. */
} TspGenerationArea;

/**
 * @brief Represents a node with x and y coordinates.
 */
typedef struct
{
    double x; /**< X-coordinate. */
    double y; /**< Y-coordinate. */
} Node;

/** Forward declaration of TspInstanceState. */
typedef struct TspInstanceState TspInstanceState;

/** Forward declaration of TspInstance. */
typedef struct TspInstance TspInstance;

/**
 * @brief Structure representing a TSP instance.
 */
struct TspInstance
{
    TspInstanceState* state; /**< Internal state of the TSP instance. */
    /**
     * @brief Retrieves the array of edge costs.
     *
     * @param self Pointer to the TSP instance.
     * @return Pointer to the edge cost array.
     */
    const double* (*get_edge_cost_array)(const TspInstance* self);
    /**
     * @brief Returns the number of nodes in the instance.
     *
     * @param self Pointer to the TSP instance.
     * @return Number of nodes.
     */
    int (*get_number_of_nodes)(const TspInstance* self);
    /**
     * @brief Retrieves the array of nodes.
     *
     * @param self Pointer to the TSP instance.
     * @return Pointer to the nodes array.
     */
    const Node* (*get_nodes)(const TspInstance* self);
};

/**
 * @brief Initializes a random TSP instance.
 *
 * Generates a TSP instance with a given number of nodes and a random seed.
 *
 * @param number_of_nodes Number of nodes to generate.
 * @param seed Seed for random generation.
 * @param generation_area Area parameters for node generation.
 * @return Pointer to the generated TSP instance.
 */
const TspInstance* init_random_tsp_instance(int number_of_nodes,
                                            int seed,
                                            TspGenerationArea generation_area);

/**
 * @brief Initializes a TSP instance with provided nodes.
 *
 * @param nodes Pointer to an array of nodes.
 * @param number_of_nodes Number of nodes.
 * @return Pointer to the initialized TSP instance.
 */
const TspInstance* init_tsp_instance(const Node* nodes, int number_of_nodes);

#endif //TSP_INSTANCE_H
