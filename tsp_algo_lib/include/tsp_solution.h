#ifndef TSP_SOLUTION_H
#define TSP_SOLUTION_H

#include <feasibility_result.h>
#include <tsp_algorithm.h>
#include <tsp_instance.h>

/** Forward declaration of TspSolutionState. */
typedef struct TspSolutionState TspSolutionState;

/** Forward declaration of TspSolution. */
typedef struct TspSolution TspSolution;

/**
 * @brief Represents a TSP solution.
 */
struct TspSolution
{
    TspSolutionState* state; /**< Internal state. */
    /**
     * @brief Solves the TSP using a given algorithm.
     * @param self Pointer to the solution.
     * @param tsp_algorithm TSP algorithm to use.
     * @return Feasibility result.
     */
    FeasibilityResult (*const solve)(const TspSolution* self, const TspAlgorithm* tsp_algorithm);
    /**
     * @brief Checks if the solution is feasible.
     * @param self Pointer to the solution.
     * @return Feasibility result.
     */
    FeasibilityResult (*const is_feasible)(const TspSolution* self);
    /**
     * @brief Frees resources associated with the solution.
     * @param self Pointer to the solution.
     */
    void (*const free)(const TspSolution* self);
    /**
     * @brief Returns the tour.
     * @param solution Pointer to the solution.
     * @return Pointer to the tour array.
     */
    const int* (* const get_tour)(const TspSolution* solution);

    double (* const get_cost)(const TspSolution* solution);
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
