#include "cmd_options.h"
#include "cmd_option_internal.h"
#include "flag_parser.h"
#include "logger.h"
#include <stddef.h>

FlagParser *cmd_options_build_cli_parser(CmdOptions *opt) {
    FlagParser *p = flag_parser_new(opt);
    if (!p) {
        if_verbose(VERBOSE_INFO, "[Config Error] Failed to create CLI parser\n");
        return NULL;
    }

    /* Help and config-file flags are handled outside the generic registry
       because they affect global parsing behavior. */
    flag_parser_add_bool(p, "--help", "-h", "Show help", &opt->help, FLAG_OPTIONAL);
    flag_parser_add_string_owned(p, "--config", "-c", "Config INI file", &opt->config_file, FLAG_OPTIONAL);

    const OptionMeta *meta = cmd_options_get_metadata();
    const size_t count = cmd_options_get_metadata_count();
    char *base_ptr = (char *) opt;

    for (size_t i = 0; i < count; i++) {
        const OptionMeta *m = &meta[i];
        void *field_ptr = base_ptr + m->offset;

        switch (m->type) {
            case OPT_INT:
                flag_parser_add_int(p, m->cli_long, m->cli_short, m->help, (int *) field_ptr, FLAG_OPTIONAL);
                break;

            case OPT_UINT:
                flag_parser_add_uint(p, m->cli_long, m->cli_short, m->help, (unsigned int *) field_ptr, FLAG_OPTIONAL);
                break;

            case OPT_DOUBLE:
                flag_parser_add_double(p, m->cli_long, m->cli_short, m->help, (double *) field_ptr, FLAG_OPTIONAL);
                break;

            case OPT_UDOUBLE:
                flag_parser_add_udouble(p, m->cli_long, m->cli_short, m->help, (double *) field_ptr, FLAG_OPTIONAL);
                break;

            case OPT_BOOL:
                flag_parser_add_bool(p, m->cli_long, m->cli_short, m->help, (bool *) field_ptr, FLAG_OPTIONAL);
                break;

            case OPT_STRING:
                flag_parser_add_string_owned(p, m->cli_long, m->cli_short, m->help, (char **) field_ptr, FLAG_OPTIONAL);
                break;

            case OPT_TSP_MODE:
                /* Enum is treated as int for CLI purposes */
                flag_parser_add_int(p, m->cli_long, m->cli_short, m->help, (int *) field_ptr, FLAG_OPTIONAL);
                break;
        }
    }

    if_verbose(VERBOSE_DEBUG, "CLI parser built from option metadata (%zu entries)\n", count);
    return p;
}
