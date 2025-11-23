#ifndef COST_RECORDER_H
#define COST_RECORDER_H

#include <stddef.h>
#include <stdbool.h>

typedef struct CostRecorder CostRecorder;

/**
 * @brief Creates a new CostRecorder instance.
 */
CostRecorder *cost_recorder_create(size_t initial_capacity);

/**
 * @brief Destroys the CostRecorder and frees memory.
 */
void cost_recorder_destroy(CostRecorder *r);

/**
 * @brief Adds a cost value to the recorder if enabled.
 */
void cost_recorder_add(CostRecorder *r, double cost);

/**
 * @brief Returns the number of recorded costs.
 */
size_t cost_recorder_get_count(const CostRecorder *r);

/**
 * @brief Returns the pointer to the array of recorded costs.
 * WARNING: The pointer is valid only until the next add operation (which might realloc).
 */
const double *cost_recorder_get_costs(const CostRecorder *r);

/**
 * @brief Enables the recording of costs.
 */
void cost_recorder_enable(CostRecorder *r);

/**
 * @brief Disable the recording of costs.
 */
void cost_recorder_disable(CostRecorder *r);

#endif // COST_RECORDER_H
