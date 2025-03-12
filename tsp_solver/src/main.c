#include <cmd_options.h>
#include <feasibility_result.h>
#include <stdio.h>
#include <tsp_instance.h>
#include <tsp_solution.h>
#include <nearest_neighbor.h>

int main(const int argc, const char *argv[]) {
    CmdOptions* cmd_options = init_cmd_options();
    parse_cli(cmd_options, argv, argc);

    const TspGenerationArea tsp_generation_area = {
        .square_side = cmd_options->generation_area.square_side,
        .x_square = cmd_options->generation_area.x_square,
        .y_square = cmd_options->generation_area.y_square,
    };

    const TspInstance *instance = init_random_tsp_instance(cmd_options->number_of_nodes,
                                                           cmd_options->seed,
                                                           tsp_generation_area);
    TspSolution *solution = init_solution(instance);
    const TspAlgorithm* nearest_neighbor = init_nearest_neighbor(cmd_options->time_limit);
    const FeasibilityResult result = solution->solve(solution, nearest_neighbor);

    printf("%s\n", feasibility_result_to_string(result));

    return 0;
}
