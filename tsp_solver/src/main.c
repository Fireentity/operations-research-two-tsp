#include <cmd_options.h>
#include <feasibility_result.h>
#include <stdio.h>
#include <tsp_instance.h>
#include <tsp_solution.h>
#include <parsing_util.h>
#include <nearest_neighbor.h>

struct CmdOptions
{
    unsigned int number_of_nodes;
    int seed;
    TspGenerationArea generation_area;
    bool help;
    unsigned int time_limit;
};

static CmdOptions cmd_options = {
    .generation_area = {
        .square_side = 0,
        .x_square = 0,
        .y_square = 0,
    },
    .help = false,
    .number_of_nodes = 0,
    .seed = 0,
    .time_limit = 0
};

int main(const int argc, const char *argv[]) {
    parse_command_line(argc, argv);

    const TspInstance *instance = init_random_tsp_instance(cmd_options.number_of_nodes,
                                                           cmd_options.seed,
                                                           cmd_options.generation_area);
    TspSolution *solution = init_solution(instance);
    const TspAlgorithm* tsp_algorithm = init_nearest_neighbor(cmd_options.time_limit);
    const FeasibilityResult result = solve(tsp_algorithm,solution);

    if (FEASIBLE != result) {
        free_tsp_solution(solution);
        FEASIBILITY_ABORT(result);
    }

    printf("%s\n", feasibility_result_to_string(result));

    plot_solution(solution, "plot.png");

    return 0;
}
