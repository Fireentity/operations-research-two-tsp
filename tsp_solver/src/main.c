#include <cmd_options.h>
#include <feasibility_result.h>
#include <flag_parser.h>
#include <limits.h>
#include <stdio.h>
#include <tsp_instance.h>
#include <tsp_solution.h>
#include <nearest_neighbor.h>
#include <plot_util.h>
#include <stdlib.h>
#include <tabu_search.h>
#include <variable_neighborhood_search.h>

#define CONCAT(a, b) a ## b

#define HELP_MESSAGE "Usage: %s [OPTION]...\n" \
                "Solves the Traveling Salesman Problem (TSP) using various algorithms.\n" \
                "\n" \
                "Mandatory options:\n" \
                "  --nodes=N                 Number of nodes (N > 0)\n" \
                "  --seed=N                  Seed for random number generation\n" \
                "  --x-square=N              X-coordinate of the square\n" \
                "  --y-square=N              Y-coordinate of the square\n" \
                "  --square-side=N           Side length of the square\n" \
                "\n" \
                "Other options:\n" \
                "  -h, --help                 Display this help message and exit\n" \
                "  --version                  Output version information\n" \
                "  --seconds=N                Set the time limit for the algorithm execution\n" \
                "  --vns                      Enable Variable Neighborhood Search (VNS) algorithm\n" \
                "  --kick-repetitions=N       Set the number of kick repetitions for VNS\n" \
                "  --n-opt=N                  Set the n-opt repetition for a kick for VNS\n" \
                "  --nearest-neighbor         Use the Nearest Neighbor heuristic for solving TSP\n" \
                "  --tabu-search              Use the Tabu Search algorithm for solving TSP\n" \
                "  --tenure=N                 Set the tenure for Tabu Search\n" \
                "  --max-stagnation=N         Set the maximum stagnation for Tabu Search\n" \
                "  --time-limit=N             Set the time limit for algorithm execution (in seconds)\n" \
                "\n" \
                "Examples:\n" \
                "  tsp_solver --nodes 10 --seed 42 --x-square 5 --y-square 5 --square-side 10\n" \
                "  tsp_solver --nodes 20 --seed 99 --x-square 3 --y-square 3 --square-side 15\n" \
                "  tsp_solver --nodes 50 --seed 100 --x-square 10 --y-square 10 --square-side 20 --vns --kick-repetitions 10 --n-opt 5 --time-limit 300\n" \
                "  tsp_solver --nodes 100 --seed 200 --x-square 5 --y-square 5 --square-side 15 --nearest-neighbor\n" \
                "  tsp_solver --nodes 30 --seed 150 --x-square 5 --y-square 5 --square-side 10 --tabu-search --tenure 50 --max-stagnation 10 --time-limit 180\n" \
                "\n" \
                "Exit status:\n" \
                "  0   if the program finishes successfully\n" \
                "  1   if an error occurs\n"

#define ERROR "Internal error this message should not appear. Report it to the developer."
#define PARSE_UNKNOWN_ARG "Argument not recognized.\n\nUsage:\n" HELP_MESSAGE
#define PARSE_USAGE_ERROR HELP_MESSAGE
#define PARSE_WRONG_VALUE_TYPE "Wrong value type for the argument.\n\nUsage:\n" HELP_MESSAGE
#define PARSE_MISSING_VALUE "Missing value for the argument.\n\nUsage:\n" HELP_MESSAGE
#define PARSE_MISSING_MANDATORY_FLAG "Missing mandatory argument.\n\nUsage:\n" HELP_MESSAGE


static const char *parsing_messages[] = {
    ERROR,
    ERROR,
    PARSE_UNKNOWN_ARG,
    PARSE_USAGE_ERROR,
    PARSE_WRONG_VALUE_TYPE,
    PARSE_MISSING_VALUE,
    PARSE_MISSING_MANDATORY_FLAG
};

void run_algorithms(const TspInstance *instance, const CmdOptions *cmd_options) {
    if (cmd_options->variable_neighborhood_search) {
        const TspSolution *solution = init_solution(instance);
        const TspAlgorithm *algorithm = init_vns(cmd_options->kick_repetitions, cmd_options->n_opt, cmd_options->time_limit);
        solution->solve(solution, algorithm);
        plot_tour(solution->get_tour(solution),
                  instance->get_number_of_nodes(instance),
                  instance->get_nodes(instance),
                  "VNS-plot.png");
        printf("VNS solution: %lf\n", solution->get_cost(solution));
        solution->free(solution);
        algorithm->free(algorithm);
    }
    if (cmd_options->nearest_neighbor) {
        const TspSolution *solution = init_solution(instance);

        const TspAlgorithm *algorithm = init_nearest_neighbor(cmd_options->time_limit);
        solution->solve(solution, algorithm);
        plot_tour(solution->get_tour(solution),
                  instance->get_number_of_nodes(instance),
                  instance->get_nodes(instance),
                  "NN-plot.png");
        printf("Nearest-neighbor solution: %lf\n", solution->get_cost(solution));
        solution->free(solution);
        algorithm->free(algorithm);
    }
    if (cmd_options->tabu_search) {
        const TspSolution *solution = init_solution(instance);

        const TspAlgorithm *algorithm = init_tabu(cmd_options->tenure, cmd_options->max_stagnation, cmd_options->time_limit);
        solution->solve(solution, algorithm);
        plot_tour(solution->get_tour(solution),
                  instance->get_number_of_nodes(instance),
                  instance->get_nodes(instance),
                  "TS-plot.png");
        printf("Tabu Search solution: %lf\n", solution->get_cost(solution));
        solution->free(solution);
        algorithm->free(algorithm);
    }
}

int main(const int argc, const char *argv[]) {
    CmdOptions *cmd_options = init_cmd_options();
    const struct FlagsArray flags_array = init_flags_array();
    FlagParser *parser = init_flag_parser(flags_array);
    const ParsingResult parsing_result = parse_flags_with_parser(cmd_options, parser, argv+1);
    if (parsing_result != PARSE_SUCCESS) {
        printf("%s", parsing_messages[parsing_result]);
        return 1;
    }

    const TspInstance *instance = init_random_tsp_instance(cmd_options->number_of_nodes,
                                                           cmd_options->seed,
                                                           (TspGenerationArea){
                                                               .square_side = cmd_options->generation_area.square_side,
                                                               .x_square = cmd_options->generation_area.x_square,
                                                               .y_square = cmd_options->generation_area.y_square,
                                                           });
    run_algorithms(instance, cmd_options);
    instance->free(instance);
    free_flag_parser(parser);
    free(cmd_options);
    free_flags_array(flags_array);
    return 0;
}
