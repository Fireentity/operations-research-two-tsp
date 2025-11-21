#ifndef COSTS_PLOTTER_H
#define COSTS_PLOTTER_H

/**
 * @brief Opaque structure for tracking cost evolution.
 */
typedef struct CostsPlotter CostsPlotter;

// --- Constructor ---

/**
 * @brief Creates a new plotter.
 * @param initial_capacity Initial size of the internal buffer.
 */
CostsPlotter *costs_plotter_create(int initial_capacity);

// --- Destructor ---

void costs_plotter_destroy(CostsPlotter *plotter);

// --- Methods ---

/**
 * @brief Records a cost value. Automatically resizes if needed.
 */
void costs_plotter_add(CostsPlotter *plotter, double cost);

/**
 * @brief Generates the plot using the recorded data.
 */
void costs_plotter_plot(const CostsPlotter *plotter, const char *filename);

#endif // COSTS_PLOTTER_H
