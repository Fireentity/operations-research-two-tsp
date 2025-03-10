#include <stdio.h>
#include <stdlib.h>
#include "parsing_util.h"
#include "single_flag.h"
#include "tsp_instance.h"
#include "tsp_solution.h"
#include "nearest_neighbor.h"



int main(const int argc, const char* argv[])
{
    parse_command_line(argc, argv);

    const CmdOptions cmd_options = get_cmd_options();
    const TspInstance* instance = init_random_tsp_instance(cmd_options.number_of_nodes,
                                                           cmd_options.seed,
                                                           cmd_options.generation_area);
    TspSolution* solution = init_solution(instance);

    const FeasibilityResult feasibility_result = solve_tsp_for_seconds(solve_with_nearest_neighbor_and_two_opt,
                                                                       solution,
                                                                       cmd_options.seconds);

    if (feasibility_result != FEASIBLE)
    {
        FEASIBILITY_ABORT(feasibility_result);
    }

    printf("%s\n", feasibility_result_to_string(feasibility_result));

    plot_solution(solution, "plot.png");

    return 0;
}