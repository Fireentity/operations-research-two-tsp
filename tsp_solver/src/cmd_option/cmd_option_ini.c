#include "cmd_options.h"
#include "cmd_option_internal.h"
#include "ini.h"
#include "parsing_util.h"
#include "logger.h"
#include <string.h>
#include <stdlib.h>

#include "c_util.h"

typedef struct {
    CmdOptions *opt;
} IniContext;

/* Strings from INI override defaults, so existing ones must be released first. */
static void handle_string(const char *value, char **dest) {
    tsp_free(*dest);
    *dest = NULL;
    parse_string(value, dest);
}

/* INI may express FILE mode in multiple textual forms. */
static void handle_tsp_mode(const char *v, void *dst) {
    TspInputMode *m = dst;
    *m = strcmp(v, "file") == 0 || strcmp(v, "FILE") == 0 || strcmp(v, "1") == 0
             ? TSP_INPUT_MODE_FILE
             : TSP_INPUT_MODE_RANDOM;
}

static int config_ini_handler(void *user, const char *section, const char *name, const char *value) {
    const IniContext *ctx = user;
    CmdOptions *opt = ctx->opt;
    char *base_ptr = (char *) opt;

    const OptionMeta *meta = cmd_options_get_metadata();
    const size_t count = cmd_options_get_metadata_count();

    /* Linear registry scan keeps INI flexible and robust to ordering. */
    for (size_t i = 0; i < count; i++) {
        const OptionMeta *m = &meta[i];

        if (strcmp(section, m->ini_section) == 0 &&
            strcmp(name, m->ini_name) == 0) {
            void *field = base_ptr + m->offset;

            switch (m->type) {
                case OPT_INT:
                    parse_int(value, field);
                    break;
                case OPT_UINT:
                    parse_uint(value, field);
                    break;
                case OPT_DOUBLE:
                    parse_double(value, field);
                    break;
                case OPT_UDOUBLE:
                    parse_udouble(value, field);
                    break;
                case OPT_BOOL:
                    *(bool *) field = (strcmp(value, "true") == 0 ||
                                       strcmp(value, "1") == 0);
                    break;
                case OPT_STRING:
                    handle_string(value, field);
                    break;
                case OPT_TSP_MODE:
                    handle_tsp_mode(value, field);
                    break;
            }
            return 1;
        }
    }

    /* Unknown INI keys are ignored on purpose to maintain forward compatibility. */
    return 1;
}

void cmd_options_parse_ini_file(CmdOptions *dest, const char *filename) {
    IniContext ctx = {dest};
    const int r = ini_parse(filename, config_ini_handler, &ctx);

    if_verbose(VERBOSE_DEBUG, "INI: parsing '%s' finished with code %d\n", filename, r);

    if (r == 0) {
        if_verbose(VERBOSE_INFO, "INI: file '%s' parsed successfully\n", filename);
    } else if (r == -1) {
        if_verbose(VERBOSE_INFO, "INI: could not open '%s'\n", filename);
    } else {
        if_verbose(VERBOSE_INFO, "INI: error at line %d in '%s'\n", r, filename);
    }
}
