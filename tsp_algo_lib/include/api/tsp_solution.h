#ifndef TSP_SOLUTION_H
#define TSP_SOLUTION_H

#include <stdbool.h>
#include "feasibility_result.h"

// Forward declarations to break include cycles
typedef struct TspAlgorithm TspAlgorithm;
typedef struct TspInstance TspInstance;
typedef struct CostsPlotter CostsPlotter;

/**
 * @brief Opaque structure representing a TSP solution.
 * The internal definition is hidden in the .c file.
 */
typedef struct TspSolution TspSolution;

// --- Constructors ---

/**
 * @brief Initializes a TSP solution for the given instance.
 * Creates a default identity tour (0, 1, 2, ..., 0).
 *
 * @param instance Pointer to the TSP instance.
 * @return Pointer to the new solution.
 */
TspSolution *tsp_solution_create(const TspInstance *instance);

/**
 * @brief Initializes a TSP solution with a specific tour.
 *
 * @param instance Pointer to the TSP instance.
 * @param tour Pointer to the tour array to copy.
 * @return Pointer to the new solution.
 */
TspSolution *tsp_solution_create_with_tour(const TspInstance *instance, const int *tour);

// --- Destructor ---

/**
 * @brief Frees all memory associated with the solution.
 * @param self Pointer to the solution.
 */
void tsp_solution_destroy(TspSolution *self);

// --- Core Operations ---

/**
 * @brief Solves the TSP instance using the provided algorithm.
 * Updates the current solution in-place if a better one is found.
 *
 * @param self The solution object (will be updated).
 * @param tsp_algorithm The algorithm to run.
 * @param plotter The plotter for visualization.
 * @return FeasibilityResult indicating if the final solution is valid.
 */
FeasibilityResult tsp_solution_solve(TspSolution *self,
                                     const TspAlgorithm *tsp_algorithm,
                                     const CostsPlotter *plotter);

/**
 * @brief Checks if the current solution is feasible.
 * Verifies loop closure, node uniqueness, and cost consistency.
 *
 * @param self The solution to check.
 * @return FEASIBLE if valid, specific error code otherwise.
 */
FeasibilityResult tsp_solution_check_feasibility(TspSolution *self);

// --- Accessors (Thread-Safe) ---

/**
 * @brief Gets the current cost, thread-safe.
 */
double tsp_solution_get_cost(TspSolution *self);

/**
 * @brief Safely copies the current best tour into the provided buffer.
 * @param self Pointer to the solution.
 * @param tour_buffer A pre-allocated buffer of size (number_of_nodes + 1).
 */
void tsp_solution_get_tour(TspSolution *self, int *tour_buffer);

/**
 * @brief Atomically updates the solution's tour and cost if the new cost is better.
 *
 * @param self Pointer to the solution.
 * @param new_tour The challenging tour.
 * @param new_cost The cost of the challenging tour.
 * @return true if the solution was updated, false otherwise.
 */
bool tsp_solution_update_if_better(TspSolution *self, const int *new_tour, double new_cost);

#endif // TSP_SOLUTION_H
