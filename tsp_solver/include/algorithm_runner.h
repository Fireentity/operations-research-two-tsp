#ifndef ALGORITHM_RUNNER_H
#define ALGORITHM_RUNNER_H

#include "cmd_option/cmd_options.h"
#include "tsp_instance.h"

/**
 * @brief Runs all algorithms selected in the options on the given instance.
 *
 * @param instance The TSP instance to run the algorithms on.
 * @param cmd_options The command-line options specifying which
 * algorithms to run and their parameters.
 */
void run_selected_algorithms(const TspInstance *instance, const CmdOptions *cmd_options);

#endif // ALGORITHM_RUNNER_H