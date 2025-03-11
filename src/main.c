#include <stdio.h>
#include <stdlib.h>
#include "parsing_util.h"
#include "tsp_instance.h"
#include "tsp_solution.h"
#include "nearest_neighbor.h"

int main(const int argc, const char *argv[]) {
    parse_command_line(argc, argv);

    const CmdOptions cmd_options = get_cmd_options();
    const TspInstance *instance = init_random_tsp_instance(cmd_options.number_of_nodes,
                                                           cmd_options.seed,
                                                           cmd_options.generation_area);
    TspSolution *solution = init_solution(instance);
    const TspAlgorithm* tsp_algorithm = init_nearest_neighbor(cmd_options.time_limit);
    const FeasibilityResult result = solve(tsp_algorithm,solution);

    if (FEASIBLE != result) {
        FEASIBILITY_ABORT(result);
    }

    printf("%s\n", feasibility_result_to_string(result));

    plot_solution(solution, "plot.png");

    return 0;
}