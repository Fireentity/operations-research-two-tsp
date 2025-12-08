#include "tsp_parser_instance.h"
#include "tsp_parser_utils.h"
#include "tsp_parser.h"
#include <string.h>

int parse_instance_tsplib(const char *path, Node **out_nodes, int *out_n) {
    FILE *f = fopen(path, "r");
    if (!f) return PARSE_ERR_OPEN;

    char token[128];
    int n = 0;
    Node *nodes = NULL;
    int section_nodes = 0;
    int err = PARSE_ERR_FORMAT;

    while (read_next_token(f, token, sizeof(token)) == 0) {

        if (!section_nodes) {
            if (strcmp(token, "DIMENSION") == 0) {
                if (read_next_token(f, token, sizeof(token)) != 0) break;
                if (strcmp(token, ":") == 0)
                    if (read_next_token(f, token, sizeof(token)) != 0) break;

                if (parse_int(token, &n) != 0 || n <= 0) goto cleanup;
            }
            else if (strcmp(token, "NODE_COORD_SECTION") == 0) {
                if (n <= 0) goto cleanup;
                nodes = malloc(n * sizeof(Node));
                if (!nodes) { err = PARSE_ERR_MEMORY; goto cleanup; }
                section_nodes = 1;
                err = PARSE_OK;
            }
            else if (strcmp(token, "EOF") == 0) break;
        }

        else {
            if (strcmp(token, "EOF") == 0) break;

            int id;
            if (parse_int(token, &id) != 0) goto cleanup;
            if (id < 1 || id > n) goto cleanup;

            if (read_next_token(f, token, sizeof(token)) != 0 ||
                parse_double(token, &nodes[id - 1].x) != 0) goto cleanup;

            if (read_next_token(f, token, sizeof(token)) != 0 ||
                parse_double(token, &nodes[id - 1].y) != 0) goto cleanup;
        }
    }

    fclose(f);

    if (!nodes || !section_nodes) {
        free(nodes);
        return PARSE_ERR_FORMAT;
    }

    *out_nodes = nodes;
    *out_n = n;
    return PARSE_OK;

    cleanup:
        fclose(f);
    free(nodes);
    return err;
}
