#include "tsp_parser_instance.h"
#include "tsp_parser_utils.h"
#include "tsp_parser.h"
#include <string.h>

#include "c_util.h"

TspParserStatus parse_instance_tsplib(const char *path, Node **out_nodes, int *out_n) {
    FILE *f = fopen(path, "r");
    if (!f) return PARSE_ERR_OPEN;

    char token[128];
    int n = 0;
    int have_nodes_section = 0;
    Node *nodes = NULL;

    TspParserStatus status = PARSE_ERR_FORMAT;

    while (read_next_token(f, token, sizeof(token)) == 0) {
        size_t len = strlen(token);
        if (len && token[len - 1] == ':')
            token[len - 1] = '\0';
        if (!token[0])
            continue;

        if (!have_nodes_section) {
            if (strcmp(token, "DIMENSION") == 0) {
                if (read_next_token(f, token, sizeof(token)) != 0) break;
                if (strcmp(token, ":") == 0 &&
                    read_next_token(f, token, sizeof(token)) != 0)
                    break;

                if (parse_int(token, &n) != 0 || n <= 0) goto fail;
            } else if (strcmp(token, "NODE_COORD_SECTION") == 0) {
                if (n <= 0) goto fail;
                nodes = tsp_malloc(n * sizeof(Node));
                if (!nodes) {
                    status = PARSE_ERR_MEMORY;
                    goto fail;
                }
                have_nodes_section = 1;
                status = PARSE_OK;
            } else if (strcmp(token, "EOF") == 0) break;
        } else {
            if (strcmp(token, "EOF") == 0) break;

            int id;
            if (parse_int(token, &id) != 0 || id < 1 || id > n) {
                status = PARSE_ERR_INV;
                goto fail;
            }

            if (read_next_token(f, token, sizeof(token)) != 0 ||
                parse_double(token, &nodes[id - 1].x) != 0) {
                status = PARSE_ERR_INV;
                goto fail;
            }

            if (read_next_token(f, token, sizeof(token)) != 0 ||
                parse_double(token, &nodes[id - 1].y) != 0) {
                status = PARSE_ERR_INV;
                goto fail;
            }
        }
    }

    fclose(f);

    if (!nodes || !have_nodes_section) {
        tsp_free(nodes);
        return PARSE_ERR_FORMAT;
    }

    *out_nodes = nodes;
    *out_n = n;
    return PARSE_OK;

fail:
    fclose(f);
    tsp_free(nodes);
    return status;
}
