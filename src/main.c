#include <stdio.h>
#include <stdlib.h>
#include "flag.h"
#include "parsing_util.h"
#include "tsp_instance.h"
#include "tsp_solution.h"
#include "nearest_neighbor.h"

// Function to parse the command-line arguments using flags.
ParsingResult parse_command_line(
    const Flag** tsp_flags,
    const int tsp_flag_size,
    const int argc,
    const char** argv)
{
    if (argc < 2)
        return PARSE_USAGE_ERROR;

    int mandatory_flags = 0;
    for (int i = 0; i < tsp_flag_size; i++)
    {
        if (tsp_flags[i]->mandatory) mandatory_flags++;
    }

    int parsed_mandatory_flags = 0;
    // Iterate through flags; stop at argc - 1 to ensure a following argument exists.
    for (int current_argv_parameter = 1; current_argv_parameter < argc; current_argv_parameter++)
    {
        for (int i = 0; i < tsp_flag_size; i++)
        {
            // Parse the flag with its associated value.
            const ParsingResult result = parse(tsp_flags[i], argv, &current_argv_parameter);
            if (result == PARSE_SUCCESS)
            {
                if (tsp_flags[i]->mandatory)
                {
                    parsed_mandatory_flags++;
                }
                break;
            }

            /*
            if(i == tsp_flag_size){
                //TODO unrecognized flag
            }
            */
        }
    }

    if (parsed_mandatory_flags != mandatory_flags)
        return PARSE_MISSING_MANDATORY_FLAG;

    return PARSE_SUCCESS;
}

int main(const int argc, const char* argv[])
{
    const Flag* tsp_flags[] = {
        init_flag("--nodes", 1, set_nodes, true),
        init_flag("--seed", 1, set_seed, false),
        init_flag("--x-square", 1, set_x_square, true),
        init_flag("--y-square", 1, set_y_square, true),
        init_flag("--square-side", 1, set_square_side, true),
        init_flag("--seconds", 1, set_seconds, false),
        init_flag("--help", 0, set_help, false)
    };
    const ParsingResult parse_result = parse_command_line(tsp_flags, 6, argc, argv);
    if (PARSE_SUCCESS != parse_result)
    {
        printf("Command line parsing, reason : %s\n", parsing_result_to_string(parse_result));
        exit(EXIT_FAILURE);
    }

    const CmdOptions cmd_options = get_cmd_options();
    const TspInstance *instance = init_random_tsp_instance(cmd_options.number_of_nodes,
                                                           cmd_options.seed,
                                                           cmd_options.generation_area);
    TspSolution *solution = init_solution(instance);

    const FeasibilityResult feasibility_result = solve_tsp_for_seconds(solve_with_nearest_neighbor_and_two_opt,
                                                                       solution,
                                                                       cmd_options.seconds);

    if (FEASIBLE != feasibility_result) {
        FEASIBILITY_ABORT(feasibility_result);
    }

    printf("%s\n", feasibility_result_to_string(feasibility_result));

    plot_solution(solution, "plot.png");

    return 0;
}