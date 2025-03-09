#include <stdio.h>
#include <stdlib.h>

#include "flag.h"
#include "parsing_util.h"
#include "single_flag.h"
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
            const ParsingResult result = parse_flag(tsp_flags[i], argv, &current_argv_parameter);
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
        init_single_flag("--nodes", set_nodes, true),
        init_single_flag("--seed", set_seed, false),
        init_single_flag("--x-square", set_x_square, true),
        init_single_flag("--y-square", set_y_square, true),
        init_single_flag("--square-side", set_square_side, true),
        init_empty_flag("--help", set_help, false)
    };

    parse_command_line(tsp_flags, 6, argc, argv);
    const CmdOptions cmd_options = get_cmd_options();
    const TspInstance* instance = init_random_tsp_instance(cmd_options.number_of_nodes,
                                                           cmd_options.seed,
                                                           cmd_options.generation_area);
    TspSolution* solution = init_solution(instance);
    const FeasibilityResult result = solve_tsp(solve_with_nearest_neighbor, solution);


    plot_solution(solution, "pre.png");
    //TODO two_opt(solution);
    plot_solution(solution, "post.png");

    if (result != FEASIBLE)
    {
        printf("Nearest Neighbor generated an unfeasible solution : %s\n", feasibility_result_to_string(result));
        exit(EXIT_FAILURE);
    }

    plot_solution(solution, NULL);

    printf("%s", feasibility_result_to_string(result));

    return 0;
}


void debug_print_nodes(Node* nodes, unsigned long count)
{
    for (unsigned long i = 0; i < count; i++)
        printf("Node[%zu]: x = %ld, y = %ld\n", i, nodes[i].x, nodes[i].y);
}
