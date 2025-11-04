#ifndef TSP_SOLUTION_H
#define TSP_SOLUTION_H

#include <stdbool.h> // For bool
#include "feasibility_result.h"

// --- Forward Declarations to break include cycles ---
typedef struct TspAlgorithm TspAlgorithm;
typedef struct TspInstance TspInstance;
typedef struct CostsPlotter CostsPlotter;

/** Forward declaration of TspSolutionState. */
typedef struct TspSolutionState TspSolutionState;

/**
 * @brief Represents a TSP solution.
 */
typedef struct TspSolution TspSolution;

struct TspSolution {
    TspSolutionState* state; /**< Internal state. */

    FeasibilityResult (*const solve)(const TspSolution* self, const TspAlgorithm* tsp_algorithm,
                                     const CostsPlotter* plotter);
    FeasibilityResult (*const is_feasible)(const TspSolution* self);
    void (*const free)(const TspSolution* self);

    /**
     * @brief Gets the current cost, thread-safe.
     */
    double (* const get_cost)(const TspSolution* solution);

    /**
     * @brief Safely copies the current best tour into the provided buffer.
     * @param self Pointer to the solution.
     * @param tour_buffer A pre-allocated buffer of size (number_of_nodes + 1).
     */
    void (*const get_tour_copy)(const TspSolution* self, int* tour_buffer);

    /**
     * @brief Atomically updates the solution's tour and cost if the new cost is better.
     * @param self Pointer to the solution.
     * @param new_tour The challenging tour.
     * @param new_cost The cost of the challenging tour.
     * @return true if the solution was updated, false otherwise.
     */
    bool (*const update_if_better)(const TspSolution* self, const int* new_tour, double new_cost);
};

/**
 * @brief Initializes a TSP solution for the given instance.
 * @param instance Pointer to the TSP instance.
 * @return Pointer to the new solution.
 */
TspSolution* init_solution(const TspInstance* instance);

/**
 * @brief Initializes a TSP solution with a provided tour.
 * @param instance Pointer to the TSP instance.
 * @param tour Pointer to the tour array.
 * @return Pointer to the new solution.
 */
TspSolution* init_solution_with_tour(const TspInstance* instance, int* tour);

#endif //TSP_SOLUTION_H
