#ifndef TSP_INSTANCE_H
#define TSP_INSTANCE_H
#include <stddef.h>

/* Error codes for instance loading */
typedef enum {
    TSP_OK = 0,
    TSP_ERR_FILE_OPEN,
    TSP_ERR_PARSE_HEADER, // Dimension or format error
    TSP_ERR_PARSE_NODES, // Error reading node coordinates
    TSP_ERR_MEMORY,
    TSP_ERR_INVALID_EXT,
    TSP_ERR_UNKNOWN_FORMAT
} TspError;

/**
 * @brief Defines the area used for generating a random TSP instance.
 */
typedef struct {
    const int x_square; /**< X-coordinate of the square grid. */
    const int y_square; /**< Y-coordinate of the square grid. */
    const unsigned int square_side; /**< Side length of the square. */
} TspGenerationArea;

/**
 * @brief Represents a node with x and y coordinates.
 */
typedef struct {
    double x;
    double y;
} Node;

/**
 * @brief Opaque TSP instance type.
 * Definition is hidden in the .c file.
 */
typedef struct TspInstance TspInstance;

/* --- Constructors --- */

/**
 * @brief Creates a random TSP instance.
 * @return Pointer to the generated TSP instance.
 */
TspInstance *tsp_instance_create_random(size_t number_of_nodes,
                                        TspGenerationArea area);

/**
 * @brief Creates a TSP instance from an existing array of nodes.
 * Makes a deep copy of the nodes.
 */
TspInstance *tsp_instance_create(const Node *nodes, int number_of_nodes);

/**
 * @brief Loads a TSP instance from a file (TSPLIB format).
 *
 * @param out_instance [Output] Pointer to the created instance variable.
 * @param path Path to the .tsp file.
 * @return TSP_OK on success, or an error code.
 */
TspError tsp_instance_load_from_file(TspInstance **out_instance, const char *path);

/* --- Destructor --- */

/**
 * @brief Frees all memory associated with the instance.
 */
void tsp_instance_destroy(TspInstance *instance);

/* --- Accessors --- */

int tsp_instance_get_num_nodes(const TspInstance *instance);

const Node *tsp_instance_get_nodes(const TspInstance *instance);

/**
 * @brief Returns the flattened edge-cost matrix (size n*n).
 */
const double *tsp_instance_get_cost_matrix(const TspInstance *instance);

/**
 * @brief Helper to get a string representation of the error.
 */
const char *tsp_error_to_string(TspError err);

#endif // TSP_INSTANCE_H