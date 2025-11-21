#include "tsp_parser.h"
#include "tsp_parser_tsplib.h"
#include "c_util.h"

#include <string.h>

typedef struct {
    const char *ext;
    TspParserFn fn;
} ParserRegistryEntry;

/* Register parsers here */
static ParserRegistryEntry parser_registry[] = {
    {".tsp", tsp_parse_tsplib},
    /* future formats: { ".csv", tsp_parse_csv }, */
    {NULL, NULL}
};

TspError tsp_parse_by_extension(const char *path, Node **out_nodes, int *out_n) {
    const char *ext = strrchr(path, '.');
    if (!ext) return TSP_ERR_INVALID_EXT;

    for (int i = 0; parser_registry[i].ext != NULL; i++) {
        if (strcasecmp(ext, parser_registry[i].ext) == 0)
            return parser_registry[i].fn(path, out_nodes, out_n);
    }

    return TSP_ERR_UNKNOWN_FORMAT;
}