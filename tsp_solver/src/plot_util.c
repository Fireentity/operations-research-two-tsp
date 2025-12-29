#include "../include/plot_util.h"
#include <stdio.h>
#include <c_util.h>
#include <sys/stat.h>
#include <string.h>
#include "constants.h"

static void ensure_dir(const char *path) {
    char buf[512];
    strcpy(buf, path);
    char *slash = strrchr(buf, '/');
    if (!slash) return;
    *slash = '\0';
    mkdir(buf, 0755);
}

static Bounds calculate_bounds(const double *x, const double *y, const size_t length) {
    Bounds b;
    if (length <= 0) {
        b.min_x = b.max_x = 0;
        b.min_y = b.max_y = 0;
        return b;
    }
    b.min_x = b.max_x = x[0];
    b.min_y = b.max_y = y[0];
    for (size_t i = 1; i < length; i++) {
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

void plot_tour(const int *tour, const int number_of_nodes, const Node *nodes, const char *output_name) {
    if (!output_name)
        output_name = "tsp_solution.png";

    ensure_dir(output_name);

    FILE *gp = popen("gnuplot", "w");
    check_popen(gp);
    fprintf(gp, "set terminal png size 800,600\n");

    double *x = tsp_malloc(sizeof(double) * (number_of_nodes + 1));

    double *y = tsp_malloc(sizeof(double) * (number_of_nodes + 1));


    for (int i = 0; i < number_of_nodes; i++) {
        x[i] = nodes[tour[i]].x;
        y[i] = nodes[tour[i]].y;
    }

    x[number_of_nodes] = nodes[tour[0]].x;
    y[number_of_nodes] = nodes[tour[0]].y;

    const Bounds bounds = calculate_bounds(x, y, number_of_nodes + 1);
    tsp_free(x);
    tsp_free(y);

    fprintf(gp, "set xrange [%lf:%lf]\n", bounds.min_x, bounds.max_x);
    fprintf(gp, "set yrange [%lf:%lf]\n", bounds.min_y, bounds.max_y);
    fprintf(gp, "set output '%s'\n", output_name);
    fprintf(gp, "plot '-' with linespoints lw 2 pt 7 notitle\n");

    for (int i = 0; i < number_of_nodes + 1; i++)
        fprintf(gp, "%lf %lf\n", nodes[tour[i % number_of_nodes]].x, nodes[tour[i % number_of_nodes]].y);

    fprintf(gp, "e\n");
    fflush(gp);
    check_pclose(pclose(gp));
}

void plot_costs_evolution(const double *costs, const size_t length, const char *output_name) {
    if (length <= 0) return;

    if (!output_name)
        output_name = "costs_evolution.png";

    ensure_dir(output_name);

    FILE *gp = popen("gnuplot", "w");
    check_popen(gp);
    fprintf(gp, "set terminal png size 1920,1080\n");
    fprintf(gp, "set xlabel 'Time'\nset ylabel 'Cost'\n");

    double *time = tsp_malloc(sizeof(double) * length);

    for (size_t i = 0; i < length; i++)
        time[i] = (double) i;

    const Bounds bounds = calculate_bounds(time, costs, length);
    tsp_free(time);

    fprintf(gp, "set xrange [%lf:%lf]\n", bounds.min_x, bounds.max_x);
    fprintf(gp, "set yrange [%lf:%lf]\n", bounds.min_y, bounds.max_y);
    fprintf(gp, "set output '%s'\n", output_name);

    double *cum_min = tsp_malloc(sizeof(double) * length);

    cum_min[0] = costs[0];
    for (size_t i = 1; i < length; i++)
        cum_min[i] = costs[i] < cum_min[i - 1] ? costs[i] : cum_min[i - 1];

    fprintf(gp, "plot '-' with linespoints lw 2 pt 7 notitle, '-' with lines lw 2 lc rgb 'red' title 'Best so far'\n");

    for (size_t i = 0; i < length; i++)
        fprintf(gp, "%lu %lf\n", i, costs[i]);
    fprintf(gp, "e\n");

    for (size_t i = 0; i < length; i++)
        fprintf(gp, "%lu %lf\n", i, cum_min[i]);
    fprintf(gp, "e\n");

    fflush(gp);
    check_pclose(pclose(gp));
    tsp_free(cum_min);
}
