#include "tsp_parser_solution.h"
#include "tsp_parser_utils.h"
#include "tsp_parser.h"
#include <string.h>
#include <stdint.h>

static int validate_tour_integrity(const int *tour, int n) {
    uint8_t *seen = calloc(n, sizeof(uint8_t));
    if (!seen) return PARSE_ERR_MEMORY;

    int err = 0;
    for (int i = 0; i < n; i++) {
        int v = tour[i];
        if (v < 0 || v >= n) { err = 1; break; }
        if (seen[v]) { err = 2; break; }
        seen[v] = 1;
    }

    if (err == 0 && tour[n] != tour[0]) err = 3;

    free(seen);
    return err == 0 ? PARSE_OK : PARSE_ERR_INV;
}

int parse_solution_v1(const char *path, int n_expected, int *tour_buf, double *out_cost) {
    FILE *f = fopen(path, "r");
    if (!f) return PARSE_ERR_OPEN;

    char buf[256];
    double cost_file = 0.0;
    int dim_file = 0;

    if (read_next_token(f, buf, sizeof(buf)) != 0 || strcmp(buf, SOL_V1_MAGIC) != 0) {
        fclose(f); return PARSE_ERR_FORMAT;
    }

    if (read_next_token(f, buf, sizeof(buf)) != 0 || strcmp(buf, SOL_V1_COST) != 0 ||
        read_next_token(f, buf, sizeof(buf)) != 0 || parse_double(buf, &cost_file) != 0) {
        fclose(f); return PARSE_ERR_FORMAT;
    }

    if (read_next_token(f, buf, sizeof(buf)) != 0 || strcmp(buf, SOL_V1_DIM) != 0 ||
        read_next_token(f, buf, sizeof(buf)) != 0 || parse_int(buf, &dim_file) != 0) {
        fclose(f); return PARSE_ERR_FORMAT;
    }

    if (dim_file != n_expected) {
        fclose(f); return PARSE_ERR_DIM;
    }

    if (read_next_token(f, buf, sizeof(buf)) != 0 || strcmp(buf, SOL_V1_TOUR_SECTION) != 0) {
        fclose(f); return PARSE_ERR_FORMAT;
    }

    for (int i = 0; i <= n_expected; i++) {
        if (read_next_token(f, buf, sizeof(buf)) != 0 || parse_int(buf, &tour_buf[i]) != 0) {
            fclose(f); return PARSE_ERR_FORMAT;
        }
    }

    int valid_res = validate_tour_integrity(tour_buf, n_expected);
    if (valid_res != PARSE_OK) {
        fclose(f); return valid_res;
    }

    *out_cost = cost_file;
    fclose(f);
    return PARSE_OK;
}
