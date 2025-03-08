#include <stdio.h>
#include <stdlib.h>
#include "tsp_instance.h"
#include "tsp_solution.h"

// Function to parse the command-line arguments using flags.
ParsingResult parse_command_line(
    const CommandFlag** tsp_flags,
    const int tsp_flag_size,
    const int argc,
    const char** argv,
    TspParams* params)
{
    if (argc < 2)
        return PARSE_USAGE_ERROR;

    int mandatory_flags = 0;
    for (int i = 0; i < tsp_flag_size; i++)
    {
        if (is_command_flag_mandatory(tsp_flags[i])) mandatory_flags++;
    }

    int parsed_mandatory_flags = 0;
    // Iterate through flags; stop at argc - 1 to ensure a following argument exists.
    for (int current_argv_parameter = 1; current_argv_parameter < argc; current_argv_parameter++)
    {
        for (int i = 0; i < tsp_flag_size; i++)
        {
            // Parse the flag with its associated value.
            const ParsingResult result = parse_flag(tsp_flags[i], argv, params, &current_argv_parameter);
            if (result == PARSE_SUCCESS)
            {
                if (is_command_flag_mandatory(tsp_flags[i]))
                {
                    parsed_mandatory_flags++;
                }
                break;
            }
        }
    }

    if (parsed_mandatory_flags != mandatory_flags)
        return PARSE_MISSING_MANDATORY_FLAG;

    return PARSE_SUCCESS;
}

int main(const int argc, const char* argv[])
{
    TspParams params;
    const CommandFlag* tsp_flags[] = {
        initialize_command_flag_with_value("--nodes", set_nodes,true),
        initialize_command_flag_with_value("--seed", set_seed,false),
        initialize_command_flag_with_value("--x-square", set_x_square,true),
        initialize_command_flag_with_value("--y-square", set_y_square,true),
        initialize_command_flag_with_value("--square-side", set_square_side,true),
        initialize_command_flag_without_value("--help", set_help,false)
    };

    parse_command_line(tsp_flags, 6, argc, argv, &params);

    //const double start_seconds = second();
    const TspInstance* instance = initialize_random_tsp_instance(&params);
    const TspSolution* solution = initialize_solution(instance);
    const FeasibilityResult result = solve_with_nearest_neighbor(solution);

    if (result != FEASIBLE)
    {
        printf("Nearest Neighbor generated an unfeasible solution : %d\n", result);
        exit(EXIT_FAILURE);
    }


    printf("%s", ENUM_TO_STRING(result));

    return 0;
}

void debug_print_nodes(Node* nodes, size_t count)
{
    for (size_t i = 0; i < count; i++)
        printf("Node[%zu]: x = %ld, y = %ld\n", i, nodes[i].x, nodes[i].y);
}
