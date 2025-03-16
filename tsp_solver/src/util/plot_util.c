#include <stdio.h>
#include <plot_util.h>
#include <c_util.h>
#include <tsp_math_util.h>
#include <constants.h>

void plot_tour(const int *tour, const int number_of_nodes, const Node* nodes, const char *output_name) {
    if (!output_name) {
        output_name = "tsp_solution.png"; // Default name
    }

    FILE *gp = popen("gnuplot", "w");
    check_popen(gp);

    fprintf(gp, "set terminal png size 800,600\n");

    const Bounds bounds = calculate_plot_bounds(tour, number_of_nodes, nodes);

    fprintf(gp, "set xrange [%lf:%lf]\n", bounds.min_x, bounds.max_x);
    fprintf(gp, "set yrange [%lf:%lf]\n", bounds.min_y, bounds.max_y);

    fprintf(gp, "set output '%s'\n", output_name);
    fprintf(gp, "plot '-' with linespoints lw 2 pt 7 notitle\n");

    for (int i = 0; i <= number_of_nodes; i++) {
        Node const node = nodes[tour[i]];
        fprintf(gp, "%lf %lf\n", (double) node.x, (double) node.y);
    }

    fprintf(gp, "e\n");
    fflush(gp);
    check_pclose(pclose(gp));
}

Bounds calculate_plot_bounds(const int *tour, const int number_of_nodes, const Node* nodes) {
    Bounds b;
    b.min_x = b.max_x = (double) nodes[tour[0]].x;
    b.min_y = b.max_y = (double) nodes[tour[0]].y;
    for (int i = 1; i < number_of_nodes; i++) {
        const double x = nodes[tour[i]].x;
        const double y = nodes[tour[i]].y;

        if (x < b.min_x) b.min_x = x;
        if (x > b.max_x) b.max_x = x;
        if (y < b.min_y) b.min_y = y;
        if (y > b.max_y) b.max_y = y;
    }
    b.min_x -= PLOT_PADDING;
    b.min_y -= PLOT_PADDING;
    b.max_x += PLOT_PADDING;
    b.max_y += PLOT_PADDING;
    return b;
}
