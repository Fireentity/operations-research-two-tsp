#include <stdio.h>
#include <stdlib.h>
#include "tsp_instance.h"
#include "tsp_solution.h"

// Function to parse the command-line arguments using flags.
void parse_command_line(
    const TspParam* tsp_flags,
    const int tsp_flag_size,
    const int argc,
    char** argv,
    TspParams* params)
{
    if (argc < 2)
    {
        fprintf(stderr,
                "Usage: %s [OPTION]...\n"
                "Generates and solves Traveling Salesman Problem (TSP) instances with specified parameters.\n"
                "\n"
                "Examples:\n"
                "  tsp_solver --nodes 10 --seed 42 --x-square 5 --y-square 5 --square-side 10\n"
                "  tsp_solver --nodes 20 --seed 99 --x-square 3 --y-square 3 --square-side 15\n"
                "\n"
                "Mandatory options:\n"
                "      --nodes=N               specify number of nodes (N must be > 0)\n"
                "      --seed=N                set random seed for reproducibility\n"
                "      --x-square=N            define x-coordinate of the square\n"
                "      --y-square=N            define y-coordinate of the square\n"
                "      --square-side=N         set side length of the square\n"
                "\n"
                "Other options:\n"
                "  -?, --help                 display this help message\n"
                "      --version              output version information\n",
                argv[0]);

        exit(1);
    }

    for (int i = 1; i < argc - 1; i++)
    {
        if (argc - i > 0)
        {
            parse_flag(tsp_flags, tsp_flag_size, argv[i], argv[i + 1], params);
            i++;
        }
    }
}

int main(const int argc, char* argv[])
{
    TspParams params;
    const TspParam tsp_flags[] = {
        {.label = "--nodes", .parse = set_nodes, .mandatory = true},
        {.label = "--seed", .parse = set_seed, .mandatory = false},
        {.label = "--x_square", .parse = set_x_square, .mandatory = true},
        {.label = "--y_square", .parse = set_y_square, .mandatory = true},
        {.label = "--square_side", .parse = set_square_side, .mandatory = true},
        {.label = "--help", .parse = set_help, .mandatory = false},
    };

    parse_command_line(tsp_flags, 6, argc, argv, &params);

    //const double start_seconds = second();
    TspInstance* instance = initialize_random_tsp_instance(&params);
    const TspSolution* solution = initialize_solution(instance);
    const FeasibilityResult result = solve_with_nearest_neighbor(solution);


    printf("%s", ENUM_TO_STRING(result));

    return 0;
}
