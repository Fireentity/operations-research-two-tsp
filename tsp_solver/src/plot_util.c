#include "plot_util.h"
#include <stdio.h>
#include <stdlib.h>
#include <c_util.h>
#include <sys/stat.h>
#include <string.h>
#include "constants.h"

// --- Configuration Constants ---
#define BASE_WIDTH  800
#define BASE_HEIGHT 600
// Less aggressive scaling to prevent unnecessarily massive images
#define PIXELS_PER_NODE_X 2
#define PIXELS_PER_NODE_Y 2

#define MAX_DIMENSION 4096

// --- Gnuplot Check Helper ---
static int has_gnuplot(void) {
    static int status = -1; // -1: unknown, 0: no, 1: yes

    if (status == -1) {
        // Try to run gnuplot --version and suppress output
        #ifdef _WIN32
            int ret = system("gnuplot --version > NUL 2>&1");
        #else
            int ret = system("gnuplot --version > /dev/null 2>&1");
        #endif

        if (ret == 0) {
            status = 1;
        } else {
            status = 0;
            printf("[WARNING] Gnuplot not found. Plotting disabled.\n");
        }
    }
    return status;
}

static void ensure_dir(const char *path) {
    char buf[512];
    strncpy(buf, path, sizeof(buf));
    buf[sizeof(buf) - 1] = '\0';

    char *slash = strrchr(buf, '/');
    if (!slash) return;
    *slash = '\0';

    struct stat st = {0};
    if (stat(buf, &st) == -1) {
        #ifdef _WIN32
            mkdir(buf);
        #else
            mkdir(buf, 0755);
        #endif
    }
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

    double padding_x = (b.max_x - b.min_x) * 0.05;
    double padding_y = (b.max_y - b.min_y) * 0.05;

    if (padding_x < PLOT_PADDING) padding_x = PLOT_PADDING;
    if (padding_y < PLOT_PADDING) padding_y = PLOT_PADDING;

    b.min_x -= padding_x;
    b.min_y -= padding_y;
    b.max_x += padding_x;
    b.max_y += padding_y;
    return b;
}

void plot_tour(const int *tour, const int number_of_nodes, const Node *nodes, const char *output_name) {
    if (!has_gnuplot()) return;

    if (!output_name)
        output_name = "tsp_solution.png";

    ensure_dir(output_name);

    // Calculate dynamic dimensions
    int width = BASE_WIDTH + (number_of_nodes * PIXELS_PER_NODE_X);
    int height = BASE_HEIGHT + (number_of_nodes * PIXELS_PER_NODE_Y);

    if (width > MAX_DIMENSION) width = MAX_DIMENSION;
    if (height > MAX_DIMENSION) height = MAX_DIMENSION;

    // Calculate font size proportional to width
    int font_size = 10 + (width / 150);

    // Calculate Point Size (ps) based on number of nodes
    double point_size = 1.2;
    if (number_of_nodes >= 1000) {
        point_size = 0.3; // Tiny dots for massive graphs
    } else if (number_of_nodes >= 200) {
        point_size = 0.6; // Smaller dots for medium graphs
    }

    FILE *gp = popen("gnuplot", "w");
    check_popen(gp);

    fprintf(gp, "set terminal png size %d,%d font 'Arial,%d'\n", width, height, font_size);
    fprintf(gp, "set grid\n");
    fprintf(gp, "set title 'TSP Tour (N=%d)'\n", number_of_nodes);

    double *x = tsp_malloc(sizeof(double) * (number_of_nodes + 1));
    double *y = tsp_malloc(sizeof(double) * (number_of_nodes + 1));

    for (int i = 0; i < number_of_nodes; i++) {
        x[i] = nodes[tour[i]].x;
        y[i] = nodes[tour[i]].y;
    }

    // Close the loop
    x[number_of_nodes] = nodes[tour[0]].x;
    y[number_of_nodes] = nodes[tour[0]].y;

    const Bounds bounds = calculate_bounds(x, y, number_of_nodes + 1);
    tsp_free(x);
    tsp_free(y);

    fprintf(gp, "set xrange [%lf:%lf]\n", bounds.min_x, bounds.max_x);
    fprintf(gp, "set yrange [%lf:%lf]\n", bounds.min_y, bounds.max_y);
    fprintf(gp, "set output '%s'\n", output_name);

    // Use dynamic point size (ps)
    fprintf(gp, "plot '-' with linespoints lw 2 pt 7 ps %.2f notitle\n", point_size);

    for (int i = 0; i <= number_of_nodes; i++) {
        fprintf(gp, "%lf %lf\n", nodes[tour[i % number_of_nodes]].x, nodes[tour[i % number_of_nodes]].y);
    }

    fprintf(gp, "e\n");
    fflush(gp);
    check_pclose(pclose(gp));
}

void plot_costs_evolution(const double *costs, const size_t length, const char *output_name) {
    if (!has_gnuplot()) return;
    if (length <= 0) return;

    if (!output_name)
        output_name = "costs_evolution.png";

    ensure_dir(output_name);

    FILE *gp = popen("gnuplot", "w");
    check_popen(gp);

    // Fixed high resolution for cost charts with readable font
    fprintf(gp, "set terminal png size 1920,1080 font 'Arial,16'\n");
    fprintf(gp, "set grid\n");
    fprintf(gp, "set title 'Cost Evolution'\n");
    fprintf(gp, "set xlabel 'Iteration'\nset ylabel 'Cost'\n");

    double *time = tsp_malloc(sizeof(double) * length);
    for (size_t i = 0; i < length; i++) time[i] = (double) i;

    const Bounds bounds = calculate_bounds(time, costs, length);
    tsp_free(time);

    fprintf(gp, "set xrange [%lf:%lf]\n", bounds.min_x, bounds.max_x);
    fprintf(gp, "set yrange [%lf:%lf]\n", bounds.min_y, bounds.max_y);
    fprintf(gp, "set output '%s'\n", output_name);

    double *cum_min = tsp_malloc(sizeof(double) * length);
    cum_min[0] = costs[0];
    for (size_t i = 1; i < length; i++)
        cum_min[i] = costs[i] < cum_min[i - 1] ? costs[i] : cum_min[i - 1];

    fprintf(gp, "plot '-' with lines lw 1 lc rgb 'gray' title 'Current Cost', "
                "'-' with lines lw 2 lc rgb 'red' title 'Best so far'\n");

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