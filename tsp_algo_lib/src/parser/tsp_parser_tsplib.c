#include "tsp_parser_tsplib.h"
#include "c_util.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>

static int parse_strict_int(const char *s, int *out) {
    char *end;
    const long v = strtol(s, &end, 10);
    if (end == s) return 0;
    while (isspace((unsigned char)*end)) end++;
    if (*end != '\0') return 0;
    if (v < 1 || v > INT_MAX) return 0;
    *out = (int) v;
    return 1;
}

TspError tsp_parse_tsplib(const char *path, Node **out_nodes, int *out_n) {
    FILE *f = fopen(path, "r");
    if (!f) return TSP_ERR_FILE_OPEN;

    char line[512];
    int n = -1;
    int reading = 0;
    Node *nodes = NULL;
    TspError err = TSP_OK;

    while (fgets(line, sizeof(line), f)) {
        char *p = line;
        while (isspace((unsigned char)*p)) p++;
        if (!*p) continue;

        if (!reading) {
            if (!strncmp(p, "DIMENSION", 9)) {
                char buf[64];
                char *colon = strchr(p, ':');
                if (colon) {
                    if (!parse_strict_int(colon + 1, &n)) {
                        err = TSP_ERR_PARSE_HEADER;
                        break;
                    }
                } else if (sscanf(p, "DIMENSION %63s", buf) == 1) {
                    if (!parse_strict_int(buf, &n)) {
                        err = TSP_ERR_PARSE_HEADER;
                        break;
                    }
                } else {
                    err = TSP_ERR_PARSE_HEADER;
                    break;
                }
            } else if (!strncmp(p, "NODE_COORD_SECTION", 18)) {
                if (n <= 0) {
                    err = TSP_ERR_PARSE_HEADER;
                    break;
                }
                nodes = malloc(n * sizeof(Node));
                check_alloc(nodes);
                for (int i = 0; i < n; i++) nodes[i].x = nodes[i].y = 0;
                reading = 1;
            }
        } else {
            if (!strncmp(p, "EOF", 3)) break;

            int id;
            double x, y;
            if (sscanf(p, "%d %lf %lf", &id, &x, &y) == 3) {
                if (id < 1 || id > n) {
                    err = TSP_ERR_PARSE_NODES;
                    break;
                }
                nodes[id - 1].x = x;
                nodes[id - 1].y = y;
            }
        }
    }

    fclose(f);

    if (err != TSP_OK) {
        free(nodes);
        return err;
    }

    if (!nodes) return TSP_ERR_PARSE_HEADER;

    *out_nodes = nodes;
    *out_n = n;
    return TSP_OK;
}
