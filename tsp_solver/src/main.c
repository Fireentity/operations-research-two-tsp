#include <cmd_options.h>
#include <feasibility_result.h>
#include <stdio.h>
#include <tsp_instance.h>
#include <tsp_solution.h>
#include <nearest_neighbor.h>
#include <plot_util.h>

#define CONCAT(a, b) a ## b

#define HELP_MESSAGE "Usage: %s [OPTION]...\n" \
                "Generates and solves Traveling Salesman Problem (TSP) instances with specified parameters.\n" \
                "\n" \
                "Examples:\n" \
                "  tsp_solver --nodes 10 --seed 42 --x-square 5 --y-square 5 --square-side 10\n" \
                "  tsp_solver --nodes 20 --seed 99 --x-square 3 --y-square 3 --square-side 15\n" \
                "\n" \
                "Mandatory options:\n" \
                "      --nodes=N               specify number of nodes (N must be > 0)\n" \
                "      --seed=N                set random seed for reproducibility\n" \
                "      --x-square=N            define x-coordinate of the square\n" \
                "      --y-square=N            define y-coordinate of the square\n" \
                "      --square-side=N         set side length of the square\n" \
                "\n" \
                "Other options:\n" \
                "  -?, --help                 display this help message\n" \
                "      --version              output version information\n;" \

#define ERROR "Internal error this message should not appear. Report it to the developer."
#define PARSE_UNKNOWN_ARG "Argument not recognized.\n\nUsage:\n" HELP_MESSAGE
#define PARSE_USAGE_ERROR HELP_MESSAGE
#define PARSE_WRONG_VALUE_TYPE "Wrong value type for the argument.\n\nUsage:\n" HELP_MESSAGE
#define PARSE_MISSING_VALUE "Missing value for the argument.\n\nUsage:\n" HELP_MESSAGE
#define PARSE_MISSING_MANDATORY_FLAG "Missing mandatory argument.\n\nUsage:\n" HELP_MESSAGE


static const char* parsing_messages[] = {
    ERROR,
    ERROR,
    PARSE_UNKNOWN_ARG,
    PARSE_USAGE_ERROR,
    PARSE_WRONG_VALUE_TYPE,
    PARSE_MISSING_VALUE,
    PARSE_MISSING_MANDATORY_FLAG
};

int main(const int argc, const char* argv[])
{
    CmdOptions* cmd_options = init_cmd_options();
    const ParsingResult parsing_result = parse_cli(cmd_options, argv+1, argc);
    if (parsing_result != PARSE_SUCCESS)
    {
        printf("%s",parsing_messages[parsing_result]);
        return 0;
    }

    const TspGenerationArea tsp_generation_area = {
        .square_side = cmd_options->generation_area.square_side,
        .x_square = cmd_options->generation_area.x_square,
        .y_square = cmd_options->generation_area.y_square,
    };

    const TspInstance* instance = init_random_tsp_instance(cmd_options->number_of_nodes,
                                                           cmd_options->seed,
                                                           tsp_generation_area);
    const TspSolution* solution = init_solution(instance);
    const TspAlgorithm* nearest_neighbor = init_nearest_neighbor(cmd_options->time_limit);
    const FeasibilityResult result = solution->solve(solution, nearest_neighbor);

    plot_tour(solution->get_tour(solution),
              instance->get_number_of_nodes(instance),
              instance->get_nodes(instance),
              "plot.png");

    printf("%s\n", feasibility_result_to_string(parsing_result));

    return 0;
}
