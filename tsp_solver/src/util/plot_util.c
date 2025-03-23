#include <stdio.h>
#include <plot_util.h>
#include <c_util.h>
#include <tsp_math_util.h>
#include <constants.h>

/**
 * @brief Generic boundaries calculator for plots.
 *
 * @param x Array of x coordinates.
 * @param y Array of y coordinates.
 * @param length Number of points.
 * @return Bounds with computed limits.
 */
static Bounds calculate_bounds(const double *x, const double *y, const int length) {
    Bounds b;
    if (length <= 0) {
        b.min_x = b.max_x = 0;
        b.min_y = b.max_y = 0;
        return b;
    }
    b.min_x = b.max_x = x[0];
    b.min_y = b.max_y = y[0];
    for (int i = 1; i < length; i++) {
        if (x[i] < b.min_x) b.min_x = x[i];
        if (x[i] > b.max_x) b.max_x = x[i];
        if (y[i] < b.min_y) b.min_y = y[i];
        if (y[i] > b.max_y) b.max_y = y[i];
    }
    b.min_x -= PLOT_PADDING;
    b.min_y -= PLOT_PADDING;
    b.max_x += PLOT_PADDING;
    b.max_y += PLOT_PADDING;
    return b;
}

void plot_tour(const int *tour, const int number_of_nodes, const Node* nodes, const char *output_name) {
    if (!output_name)
        output_name = "tsp_solution.png";
    FILE *gp = popen("gnuplot", "w");
    check_popen(gp);
    fprintf(gp, "set terminal png size 800,600\n");

    double *x = malloc(sizeof(double) * (number_of_nodes + 1));
    double *y = malloc(sizeof(double) * (number_of_nodes + 1));
    for (int i = 0; i < number_of_nodes; i++) {
        x[i] = nodes[tour[i]].x;
        y[i] = nodes[tour[i]].y;
    }
    /* Close the tour loop */
    x[number_of_nodes] = nodes[tour[0]].x;
    y[number_of_nodes] = nodes[tour[0]].y;

    const Bounds bounds = calculate_bounds(x, y, number_of_nodes + 1);
    free(x); free(y);

    fprintf(gp, "set xrange [%lf:%lf]\n", bounds.min_x, bounds.max_x);
    fprintf(gp, "set yrange [%lf:%lf]\n", bounds.min_y, bounds.max_y);
    fprintf(gp, "set output '%s'\n", output_name);
    fprintf(gp, "plot '-' with linespoints lw 2 pt 7 notitle\n");
    for (int i = 0; i < number_of_nodes + 1; i++) {
        fprintf(gp, "%lf %lf\n", nodes[tour[i % number_of_nodes]].x, nodes[tour[i % number_of_nodes]].y);
    }
    fprintf(gp, "e\n");
    fflush(gp);
    check_pclose(pclose(gp));
}

void plot_costs_evolution(const double *costs, const int length, const char *output_name) {
    if (!output_name)
        output_name = "costs_evolution.png";
    FILE *gp = popen("gnuplot", "w");
    check_popen(gp);
    fprintf(gp, "set terminal png size 800,600\n");
    fprintf(gp, "set xlabel 'Time'\nset ylabel 'Cost'\n");

    double *time = malloc(sizeof(double) * length);
    for (int i = 0; i < length; i++)
        time[i] = (double)i;
    const Bounds bounds = calculate_bounds(time, costs, length);
    free(time);

    fprintf(gp, "set xrange [%lf:%lf]\n", bounds.min_x, bounds.max_x);
    fprintf(gp, "set yrange [%lf:%lf]\n", bounds.min_y, bounds.max_y);
    fprintf(gp, "set output '%s'\n", output_name);
    fprintf(gp, "plot '-' with linespoints lw 2 pt 7 notitle\n");
    for (int i = 0; i < length; i++)
        fprintf(gp, "%d %lf\n", i, costs[i]);
    fprintf(gp, "e\n");
    fflush(gp);
    check_pclose(pclose(gp));
}
